#include <xviz/qt/scene/renderer.hpp>
#include <xviz/qt/scene/mesh_data.hpp>
#include <xviz/qt/scene/material.hpp>

#include <xviz/scene/scene.hpp>
#include <xviz/scene/node.hpp>
#include <xviz/scene/drawable.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/camera.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/material.hpp>

#include <QOpenGLFunctions>
#include <QImage>
#include <QOpenGLTexture>

#include <xviz/qt/resource_loader.hpp>

#include "../qt_graphics_helpers.hpp"
#include "util.hpp"
#include "shadow_map.hpp"

#include <iostream>

#define POSITION_LOCATION    0
#define NORMALS_LOCATION    1
#define COLORS_LOCATION    2
#define BONE_ID_LOCATION    3
#define BONE_WEIGHT_LOCATION    4
#define UV_LOCATION 5

using namespace Eigen ;
using namespace std ;

Renderer::Renderer(int flags) {

}

void Renderer::setupTexture(const xviz::Material *mat, const xviz::Texture2D *texture, uint slot) {
    if ( textures_.count(mat) == 0 ) {
        textures_[mat] = {nullptr} ;
    }
    if ( textures_[mat][slot] != nullptr ) return ; // already loaded

    auto &loader = ResourceLoader::instance() ;
    if ( texture ) {
        QObject::connect(&loader, &ResourceLoader::downloaded, this, [&](const QByteArray &data){
            QImage im ;
            im.loadFromData(data) ;
            uploadTexture(im, mat, slot);
        });

        xviz::Image image = texture->image() ;

        if ( image.type() == xviz::ImageType::Uri )
            loader.fetch(QString::fromStdString(image.uri())) ;
        else {
            QImage qim = qImageFromImage(image) ;
            uploadTexture(qim, mat, slot) ;
        }
    }
}

MaterialProgramPtr Renderer::instantiateMaterial(const xviz::Material *mat, int flags) {
    if ( const xviz::PhongMaterial *material = dynamic_cast<const xviz::PhongMaterial *>(mat)) {
        if ( material->diffuseTexture()  ) {
            flags |= HAS_DIFFUSE_TEXTURE ;
            setupTexture(mat, material->diffuseTexture(), 0);
        }

        return PhongMaterialProgram::instance(flags) ;
    } else if ( const xviz::ConstantMaterial *material = dynamic_cast<const xviz::ConstantMaterial *>(mat)) {
        return ConstantMaterialProgram::instance(flags) ;
    } else if ( const xviz::PerVertexColorMaterial *material = dynamic_cast<const xviz::PerVertexColorMaterial *>(mat)) {
        return PerVertexColorMaterialProgram::instance(flags) ;
    }

    return nullptr ;
}

void Renderer::init(const xviz::NodePtr &scene) {
    initializeOpenGLFunctions() ;

    initShadowMapRenderer();

    xviz::PhongMaterial *mat = new xviz::PhongMaterial() ;
    mat->setDiffuseColor({0.5, 0.5, 0.5, 1.0}) ;
    default_material_.reset(mat) ;

    scene_ = scene ;

}

Renderer::~Renderer() {

}

void Renderer::setupCulling(const xviz::Material *mat) {
    switch ( mat->side() ) {
    case xviz::Material::Side::Front:
        glEnable(GL_CULL_FACE) ;
        glCullFace(GL_BACK) ;
        break ;
    case xviz::Material::Side::Back:
        glEnable(GL_CULL_FACE) ;
        glCullFace(GL_FRONT) ;
        break ;
    case xviz::Material::Side::Both:
        glDisable(GL_CULL_FACE) ;
        break ;
    }
}

void Renderer::setupShadows(const xviz::LightPtr &light) {
    if ( !shadow_map_ ) {
        shadow_map_.reset(new ShadowMap()) ;
        shadow_map_->init(shadow_map_width_, shadow_map_height_) ;
    }
    if ( const xviz::DirectionalLight *dl = dynamic_cast<const xviz::DirectionalLight *>(light.get()) ) {
        Matrix4f lightProjection =
                ortho(light->shadow_cam_left_, light->shadow_cam_right_,
                      light->shadow_cam_top_, light->shadow_cam_bottom_,
                      light->shadow_cam_near_, light->shadow_cam_far_);
        Matrix4f lightView = lookAt(dl->position_, dl->target_, Vector3f(0.0, 1.0, 0.0));

        ls_mat_ = lightProjection * lightView;

        renderShadowMap(light);
    }
}

const MeshData *Renderer::fetchMeshData(xviz::GeometryPtr &geom) {
     MeshData *data = nullptr ;
    auto it = meshes_.find(geom.get()) ;
    if ( it == meshes_.end() ) {
        data = new MeshData(*geom) ;
        meshes_.emplace(geom.get(), std::unique_ptr<MeshData>(data)) ;
    } else
        data = (*it).second.get() ;

    data->update(*geom) ;

    return data ;

}

void Renderer::renderShadowMap(const xviz::LightPtr &l) {
    shadow_map_->bind();

    glViewport(0, 0, shadow_map_width_, shadow_map_height_);

    shadow_map_->bindTexture(GL_TEXTURE0) ;

    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    for ( const xviz::ConstNodePtr &node: scene_->getNodesRecursive() ) {
        for( const auto &dr: node->drawables() ) {
            xviz::GeometryPtr geom = dr.geometry() ;

            if ( !geom || !geom->castsShadows()) continue ;

            const MeshData *data = fetchMeshData(geom) ;

            if ( !data ) continue ;

            shadow_map_shader_.bind() ;
            shadow_map_shader_.setUniformValue(shadow_map_shader_.uniformLocation("lightSpaceMatrix"), eigenToQt(ls_mat_));
            shadow_map_shader_.setUniformValue(shadow_map_shader_.uniformLocation("model"), eigenToQt(node->globalTransform().matrix())) ;
            drawMeshData(*data, geom, true) ;
            shadow_map_shader_.release() ;
        }
    }

    shadow_map_->unbind(default_fbo_) ;
}

void Renderer::render(const xviz::CameraPtr &cam) {


    // render background

    Vector4f bg_clr = cam->bgColor() ;

    glClearColor(bg_clr.x(), bg_clr.y(), bg_clr.z(), bg_clr.w()) ;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // setup camera matrices

    if ( const auto &pcam = dynamic_pointer_cast<xviz::PerspectiveCamera>(cam) ) {
        perspective_ = pcam->projectionMatrix() ;
        znear_ = pcam->zNear() ;
        zfar_ = pcam->zFar() ;
    }

    const xviz::Viewport &vp = cam->getViewport() ;

    proj_ = cam->getViewMatrix() ;

    // setup gl

    glEnable(GL_DEPTH_TEST) ;
    glDepthFunc(GL_LEQUAL);

    glFrontFace(GL_CCW) ;
    glEnable (GL_BLEND);

    bool first_pass = true ;
    // render scene for each light
    for( const xviz::ConstNodePtr &node: scene_->getNodesRecursive() ) {
        xviz::LightPtr l = node->light() ;
        if ( l  ) {
            Affine3f ltr = node->globalTransform() ;

            if ( first_pass )
                glBlendFunc(GL_ONE, GL_ZERO);
            else
                glBlendFunc (GL_ONE, GL_ONE);

            if ( l->casts_shadows_ ) setupShadows(l) ;


            glViewport(vp.x_, vp.y_, vp.width_, vp.height_);

            renderScene(l, ltr) ;

            first_pass = false ;

        }
    }

/*
glBlendFunc(GL_ONE, GL_ZERO);
      glViewport(0, 0, 256, 256);
     renderShadowDebug() ;
  glBindTexture(GL_TEXTURE_2D, 0);
*/

}

Vector2f Renderer::project(const Vector3f &pos) {
    Vector3f p = Affine3f(perspective_ * proj_) * pos ;

    GLint vp[4] ;
    glGetIntegerv (GL_VIEWPORT, vp) ;

    float xn = p.x()/p.z(), yn = p.y()/p.z() ;
    float x = (xn + 1.0) * (vp[2]/2.0) + vp[0] ;
    float y = (yn + 1.0) * (vp[3]/2.0) + vp[1];

    return { x, vp[3] - y } ;
}


unsigned int quadVAO = 0;
unsigned int quadVBO;

void Renderer::renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


void Renderer::renderShadowDebug()
{
    shadow_map_debug_shader_.bind() ;
    shadow_map_debug_shader_.setUniformValue(shadow_map_debug_shader_.uniformLocation("depthMap"), 0);
    shadow_map_debug_shader_.setUniformValue(shadow_map_debug_shader_.uniformLocation("near_plane"), -10.f) ;
    shadow_map_debug_shader_.setUniformValue(shadow_map_debug_shader_.uniformLocation("far_plane"), 7.0f) ;

    shadow_map_->bindTexture(GL_TEXTURE0) ;
    renderQuad() ;
    shadow_map_debug_shader_.release() ;
}

void Renderer::renderScene(const xviz::LightPtr &l, const Affine3f &light_mat) {

    for ( const xviz::ConstNodePtr &node: scene_->getNodesRecursive() ) {
        if ( !node->isVisible() ) continue ;
        for( const auto &drawable: node->drawables() )
            render(drawable, node->globalTransform(), l, light_mat ) ;
    }
}

void Renderer::uploadTexture(QImage im, const xviz::Material *mat, int slot) {
    QOpenGLTexture *texture = new QOpenGLTexture(im);
    texture->setMinificationFilter(QOpenGLTexture::Linear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat) ;
    texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat) ;
    texture->generateMipMaps();

    textures_[mat][slot] = texture ;
}

/*
void Renderer::render(const xviz::NodePtr &node, const Matrix4f &tf) {

    Matrix4f mat = node->transform().matrix(),
            tr = tf * mat ; // accumulate transform

    const auto &drawables = node->drawables() ;

    for( const auto &m: drawables )
        render(m, tr) ;

    for( uint i=0 ; i<node->numChildren() ; i++ ) {
        const xviz::NodePtr &n = node->getChild(i) ;
        render(n, tr) ;
    }
}
*/

#define MAX_LIGHTS 10
/*
void Renderer::setLights(const xviz::NodePtr &node, const Affine3f &parent_tf, const MaterialProgramPtr &mat)
{
    Affine3f tf = parent_tf * node->transform() ;

    xviz::LightPtr l = node->light() ;

    if ( light_index_ >= MAX_LIGHTS ) return ;

    if ( l ) {
        mat->applyLight(light_index_, l, tf) ;
        const auto &shadow_data =  shadow_map_renderer_->data() ;
        auto it = shadow_data.find(l.get()) ;
        if ( it != shadow_data.end() ) {
            const auto &ld = (*it).second ;
            mat->applyShadow(light_index_, l, ld->ls_mat_) ;
            ld->shadow_map_.bindTexture(GL_TEXTURE0 + 4 + light_index_) ;
        }
        light_index_ ++ ;
    }

    for( uint i=0 ; i<node->numChildren() ; i++ )
        setLights(node->getChild(i), tf, mat) ;
}

void Renderer::setLights(const MaterialProgramPtr &material) {
    light_index_ = 0 ;

    Isometry3f mat ;
    mat.setIdentity() ;

    for( const xviz::NodePtr &node: scene_->children() )
        setLights(node, mat, material) ;
}

*/
void Renderer::render(const xviz::Drawable &dr, const Affine3f &mat, const xviz::LightPtr &l, const Affine3f &lmat)
{
    xviz::GeometryPtr mesh = dr.geometry() ;
    if ( !mesh ) return ;

    const MeshData *data = fetchMeshData(mesh) ;

    xviz::MaterialPtr material = dr.material() ;

    MaterialProgramPtr prog ;

    int flags = 0 ;
    if ( mesh->hasSkeleton() ) flags |= ENABLE_SKINNING ;
    if ( l->casts_shadows_ ) flags |= ENABLE_SHADOWS ;

    if ( !material ) {
        material = default_material_ ;
    }

    prog = instantiateMaterial(material.get(), flags) ;

    setupCulling(material.get()) ;

    prog->use() ;
    prog->applyParams(material) ;
    prog->applyTransform(perspective_, proj_, mat.matrix()) ;
    prog->applyLight(l, lmat, ls_mat_) ;

    // bind material textures

    auto tit = textures_.find(material.get()) ;
    if ( tit != textures_.end() ) {
        const TextureData &data = tit->second ;
        for( int i=0 ; i<4 ; i++ ) {
            QOpenGLTexture *texture = data[i] ;
            if ( texture != nullptr ) {
                texture->bind(GL_TEXTURE0 + i) ;
            }
        }
    }

    if ( l->casts_shadows_ )
        shadow_map_->bindTexture(GL_TEXTURE0 + 4) ;


    if ( mesh && mesh->hasSkeleton() )
        setPose(mesh, prog) ;

#if 0

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, data.tf_);

    glEnable(GL_RASTERIZER_DISCARD);

    glBeginTransformFeedback(GL_TRIANGLES);

    drawMeshData(*data, geom->geometry()) ;

    glEndTransformFeedback();

    glDisable(GL_RASTERIZER_DISCARD);

    vector<GLfloat> fdata(36, 0) ;

    glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 36*sizeof(GLfloat), fdata.data());

    glBindVertexArray(0) ;
#else
    drawMeshData(*data, mesh) ;

#endif
    glUseProgram(0) ;
}

#include "shaders/shadow_map.vs.hpp"
#include "shaders/shadow_map.fs.hpp"

void Renderer::initShadowMapRenderer()
{
    std::string preproc("#version 330\n") ;

    string vs_code = preproc + shadow_map_shader_vs ;
    string fs_code = preproc + shadow_map_shader_fs ;

    shadow_map_shader_.addShaderFromSourceCode(QOpenGLShader::Vertex, vs_code.c_str()) ;
    shadow_map_shader_.addShaderFromSourceCode(QOpenGLShader::Fragment, fs_code.c_str()) ;
    shadow_map_shader_.link() ;

    vs_code = preproc + shadow_debug_shader_vs ;
    fs_code = preproc + shadow_debug_shader_fs ;

    shadow_map_debug_shader_.addShaderFromSourceCode(QOpenGLShader::Vertex, vs_code.c_str()) ;
    shadow_map_debug_shader_.addShaderFromSourceCode(QOpenGLShader::Fragment, fs_code.c_str()) ;
    shadow_map_debug_shader_.link() ;
}


void Renderer::setPose(const xviz::GeometryPtr &mesh, const MaterialProgramPtr &mat) {
    const auto &skeleton = mesh->skeleton() ;
    for( int i=0 ; i<skeleton.size() ; i++ ) {
        const xviz::Geometry::Bone &b = skeleton[i] ;
        mat->applyBoneTransform(i, ( b.node_->globalTransform() * b.offset_).matrix()) ;
    }
}

void Renderer::drawMeshData(const MeshData &data, xviz::GeometryPtr mesh, bool solid) {

    data.vao_->bind() ;

    if ( mesh ) {
        if ( mesh->ptype() == xviz::Geometry::Triangles ) {
            if ( data.index_ ) {
                // bind index buffer if you want to render indexed data
                data.index_->bind() ;
                // indexed draw call
                glDrawElements(GL_TRIANGLES, data.indices_, GL_UNSIGNED_INT, nullptr);

                data.index_->release() ;
            }
            else
                glDrawArrays(GL_TRIANGLES, 0, data.elem_count_) ;
        }
        else if ( mesh->ptype() == xviz::Geometry::Lines && !solid ) {
            if ( data.index_ ) {
                // bind index buffer if you want to render indexed data
                data.index_->bind() ;
                // indexed draw call
                glDrawElements(GL_LINES, data.indices_, GL_UNSIGNED_INT, nullptr);

                data.index_->release() ;
            }
            else
                glDrawArrays(GL_LINES, 0, data.elem_count_) ;
        }
        else if ( mesh->ptype() == xviz::Geometry::Points && !solid ) {
            glDrawArrays(GL_POINTS, 0, data.elem_count_) ;
        }

    }

    data.vao_->release() ;

    glFlush();
}

