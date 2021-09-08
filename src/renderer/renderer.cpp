#include <clsim/scene/renderer.hpp>

#include "shadow_map.hpp"
#include "util.hpp"
#include "mesh_data.hpp"
#include "material_program.hpp"

//#include <xviz/qt/scene/mesh_data.hpp>
//#include <xviz/qt/scene/material.hpp>

#include <clsim/scene/scene.hpp>
#include <clsim/scene/node.hpp>
#include <clsim/scene/drawable.hpp>
#include <clsim/scene/geometry.hpp>
#include <clsim/scene/camera.hpp>
#include <clsim/scene/light.hpp>
#include <clsim/scene/material.hpp>

//#include <xviz/qt/resource_loader.hpp>

//#include "../qt_graphics_helpers.hpp"
//#include "util.hpp"
//#include "shadow_map.hpp"

#include <iostream>

#define POSITION_LOCATION    0
#define NORMALS_LOCATION    1
#define COLORS_LOCATION    2
#define BONE_ID_LOCATION    3
#define BONE_WEIGHT_LOCATION    4
#define UV_LOCATION 5

using namespace Eigen ;
using namespace std ;

namespace clsim {

using namespace impl ;

Renderer::Renderer(int flags) {
    PhongMaterial *mat = new PhongMaterial() ;
    mat->setDiffuseColor({0.5, 0.5, 0.5, 1.0}) ;
    default_material_.reset(mat) ;


}

void Renderer::setupTexture(const Material *mat, const Texture2D *texture, uint slot) {
    if ( textures_.count(mat) == 0 ) {
        textures_[mat] = {0} ;
    }
    if ( textures_[mat][slot] != 0 ) return ; // already loaded

    if ( texture ) {

        auto tlcb = [&] (const Image &im) {
            uploadTexture(im, mat, slot) ;
        };

        Image image = texture->image() ;

        if ( image.type() == ImageType::Uri )
            resource_loader_->loadTexture(image.uri(), tlcb) ;
        else {
            uploadTexture(image, mat, slot) ;
        }
    }
}

MaterialProgramPtr Renderer::instantiateMaterial(const Material *mat, int flags) {
    if ( const PhongMaterial *material = dynamic_cast<const PhongMaterial *>(mat)) {
        if ( material->diffuseTexture()  ) {
            flags |= HAS_DIFFUSE_TEXTURE ;
            setupTexture(mat, material->diffuseTexture(), 0);
        }

        return PhongMaterialProgram::instance(flags) ;
    } else if ( const ConstantMaterial *material = dynamic_cast<const ConstantMaterial *>(mat)) {
        return ConstantMaterialProgram::instance(flags) ;
    } else if ( const PerVertexColorMaterial *material = dynamic_cast<const PerVertexColorMaterial *>(mat)) {
        return PerVertexColorMaterialProgram::instance(flags) ;
    }

    return nullptr ;
}

void Renderer::init(const NodePtr &scene) {
    gl3wInit();

    meshes_.clear() ;

    scene_ = scene ;
}

Renderer::~Renderer() {

}

void Renderer::setupCulling(const Material *mat) {
    switch ( mat->side() ) {
    case Material::Side::Front:
        glEnable(GL_CULL_FACE) ;
        glCullFace(GL_BACK) ;
        break ;
    case Material::Side::Back:
        glEnable(GL_CULL_FACE) ;
        glCullFace(GL_FRONT) ;
        break ;
    case Material::Side::Both:
        glDisable(GL_CULL_FACE) ;
        break ;
    }
}

void Renderer::setupShadows(const LightPtr &light) {


    initShadowMapRenderer();


    if ( !shadow_map_ ) {
        shadow_map_.reset(new ShadowMap()) ;
        shadow_map_->init(shadow_map_width_, shadow_map_height_) ;
    }
    if ( const DirectionalLight *dl = dynamic_cast<const DirectionalLight *>(light.get()) ) {
        Matrix4f lightProjection =
                ortho(light->shadow_cam_left_, light->shadow_cam_right_,
                      light->shadow_cam_top_, light->shadow_cam_bottom_,
                      light->shadow_cam_near_, light->shadow_cam_far_);
        Matrix4f lightView = lookAt(dl->position_, dl->target_, Vector3f(0.0, 1.0, 0.0));

        ls_mat_ = lightProjection * lightView;

        renderShadowMap(light);
    }
}

const MeshData *Renderer::fetchMeshData(GeometryPtr &geom) {
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

void Renderer::renderShadowMap(const LightPtr &l) {
    shadow_map_->bind();

    glViewport(0, 0, shadow_map_width_, shadow_map_height_);

    shadow_map_->bindTexture(GL_TEXTURE0) ;

    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    for ( const ConstNodePtr &node: scene_->getNodesRecursive() ) {
        for( const auto &dr: node->drawables() ) {
            GeometryPtr geom = dr.geometry() ;

            if ( !geom || !geom->castsShadows()) continue ;

            const MeshData *data = fetchMeshData(geom) ;

            if ( !data ) continue ;
/*
            shadow_map_shader_.bind() ;
            shadow_map_shader_.setUniformValue(shadow_map_shader_.uniformLocation("lightSpaceMatrix"), eigenToQt(ls_mat_));
            shadow_map_shader_.setUniformValue(shadow_map_shader_.uniformLocation("model"), eigenToQt(node->globalTransform().matrix())) ;
            drawMeshData(*data, geom, true) ;
            shadow_map_shader_.release() ;
            */
        }
    }

    shadow_map_->unbind(default_fbo_) ;
}

void Renderer::render(const CameraPtr &cam) {


    // render background

    Vector4f bg_clr = cam->bgColor() ;

    glClearColor(bg_clr.x(), bg_clr.y(), bg_clr.z(), bg_clr.w()) ;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // setup camera matrices

    if ( const auto &pcam = dynamic_pointer_cast<PerspectiveCamera>(cam) ) {
        perspective_ = pcam->projectionMatrix() ;
        znear_ = pcam->zNear() ;
        zfar_ = pcam->zFar() ;
    }

    const Viewport &vp = cam->getViewport() ;

    proj_ = cam->getViewMatrix() ;

    // setup gl

    glEnable(GL_DEPTH_TEST) ;
    glDepthFunc(GL_LEQUAL);

    glFrontFace(GL_CCW) ;
    glEnable (GL_BLEND);

    glEnable(GL_LINE_SMOOTH) ;
    glLineWidth(1.0) ;

    bool first_pass = true ;
    // render scene for each light
    for( const ConstNodePtr &node: scene_->getNodesRecursive() ) {
        LightPtr l = node->light() ;
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
    /*
    shadow_map_debug_shader_.bind() ;
    shadow_map_debug_shader_.setUniformValue(shadow_map_debug_shader_.uniformLocation("depthMap"), 0);
    shadow_map_debug_shader_.setUniformValue(shadow_map_debug_shader_.uniformLocation("near_plane"), -10.f) ;
    shadow_map_debug_shader_.setUniformValue(shadow_map_debug_shader_.uniformLocation("far_plane"), 7.0f) ;

    shadow_map_->bindTexture(GL_TEXTURE0) ;
    renderQuad() ;
    shadow_map_debug_shader_.release() ;
    */
}

void Renderer::renderScene(const LightPtr &l, const Affine3f &light_mat) {

    for ( const ConstNodePtr &node: scene_->getNodesRecursive() ) {
        if ( !node->isVisible() ) continue ;
        for( const auto &drawable: node->drawables() )
            render(drawable, node->globalTransform(), l, light_mat ) ;
    }
}

void Renderer::uploadTexture(const Image &im, const Material *mat, int slot) {
/*    if ( im.isNull() ) return ;
    QOpenGLTexture *texture = new QOpenGLTexture(im);
    texture->setMinificationFilter(QOpenGLTexture::Linear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat) ;
    texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat) ;
    texture->generateMipMaps();

    textures_[mat][slot] = texture ;
    */
}

#define MAX_LIGHTS 10

void Renderer::render(const Drawable &dr, const Affine3f &mat, const LightPtr &l, const Affine3f &lmat)
{
    GeometryPtr mesh = dr.geometry() ;
    if ( !mesh ) return ;

    const MeshData *data = fetchMeshData(mesh) ;

    MaterialPtr material = dr.material() ;

    MaterialProgramPtr prog ;

    int flags = 0 ;
    if ( mesh->hasSkeleton() ) flags |= ENABLE_SKINNING ;
    if ( l->casts_shadows_ ) flags |= ENABLE_SHADOWS ;

    if ( !material ) {
        material = default_material_ ;
    }

    prog = instantiateMaterial(material.get(), flags) ;
    programs_.push_back(prog) ;

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
            /*
            QOpenGLTexture *texture = data[i] ;
            if ( texture != nullptr ) {
                texture->bind(GL_TEXTURE0 + i) ;
            }
            */
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
    //if ( shadow_map_shader_.isLinked() ) return ;

    std::string preproc("#version 330\n") ;

    OpenGLShaderPtr vs(new OpenGLShader(VERTEX_SHADER)) ;
    vs->addSourceFile(preproc) ;
    vs->addSourceFile(shadow_map_shader_vs) ;

    OpenGLShaderPtr fs(new OpenGLShader(FRAGMENT_SHADER)) ;
    fs->addSourceFile(preproc) ;
    fs->addSourceFile(shadow_map_shader_fs) ;

    shadow_map_shader_.reset(new OpenGLShaderProgram) ;
    shadow_map_shader_->addShader(vs) ;
    shadow_map_shader_->addShader(fs) ;
    shadow_map_shader_->link() ;

    OpenGLShaderPtr vsd(new OpenGLShader(VERTEX_SHADER)) ;
    vsd->addSourceFile(preproc) ;
    vsd->addSourceFile(shadow_debug_shader_vs) ;

    OpenGLShaderPtr fsd(new OpenGLShader(FRAGMENT_SHADER)) ;
    fsd->addSourceFile(preproc) ;
    fsd->addSourceFile(shadow_debug_shader_fs) ;

    shadow_map_debug_shader_.reset(new OpenGLShaderProgram) ;
    shadow_map_debug_shader_->addShader(vs) ;
    shadow_map_debug_shader_->addShader(fs) ;
    shadow_map_debug_shader_->link() ;

}


void Renderer::setPose(const GeometryPtr &mesh, const MaterialProgramPtr &mat) {
    const auto &skeleton = mesh->skeleton() ;
    for( int i=0 ; i<skeleton.size() ; i++ ) {
        const Geometry::Bone &b = skeleton[i] ;
        mat->applyBoneTransform(i, ( b.node_->globalTransform() * b.offset_).matrix()) ;
    }
}

void Renderer::drawMeshData(const MeshData &data, GeometryPtr mesh, bool solid) {

    /*
    data.vao_->bind() ;

    if ( mesh ) {
        if ( mesh->ptype() == Geometry::Triangles ) {
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
        else if ( mesh->ptype() == Geometry::Lines && !solid ) {
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
        else if ( mesh->ptype() == Geometry::Points && !solid ) {
            glDrawArrays(GL_POINTS, 0, data.elem_count_) ;
        }

    }

    data.vao_->release() ;
*/
    glFlush();
}

}
