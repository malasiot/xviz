#include "renderer.hpp"

#include <xviz/scene/scene.hpp>
#include <xviz/scene/node.hpp>
#include <xviz/scene/drawable.hpp>
#include <xviz/scene/mesh.hpp>
#include <xviz/scene/camera.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/material.hpp>

#include <QOpenGLFunctions>
#include <QImage>
#include <QOpenGLTexture>

#include "mesh_data.hpp"
#include "material.hpp"
#include "../image_loader.hpp"
#include "../qt_graphics_helpers.hpp"

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

void Renderer::init(const xviz::ScenePtr &scene) {
    initializeOpenGLFunctions() ;

    auto &loader = ImageLoader::instance() ;

    xviz::PhongMaterial *mat = new xviz::PhongMaterial() ;
    mat->setDiffuseColor({0.5, 0.5, 0.5, 1.0}) ;
    default_material_.reset(mat) ;
    default_prog_ = PhongMaterialProgram::instance(0) ;

    scene_ = scene ;

    for ( const xviz::MeshPtr &mesh: scene->meshes() ) {
        MeshData *data = new MeshData(*mesh) ;
        meshes_.emplace(make_pair(mesh.get(), data)) ;
    }

    for ( const xviz::MaterialPtr &mat: scene->materials() ) {

        if ( const xviz::PhongMaterial *material = dynamic_cast<const xviz::PhongMaterial *>(mat.get())) {
            int flags = 0 ;

            textures_[(xviz::Material *)material] = {nullptr} ;

            if ( material->diffuseTexture() ) {
                flags |= PhongMaterialProgram::HAS_DIFFUSE_TEXTURE ;
                const xviz::Texture2D *texture = material->diffuseTexture() ;

                QObject::connect(&loader, &ImageLoader::downloaded, this, [&](QImage im){ uploadTexture(im, mat, 0);});

                xviz::Image image = texture->image() ;

                if ( image.type() == xviz::ImageType::Uri )
                    loader.fetch(QString::fromStdString(image.uri())) ;
                else {
                    QImage qim = qImageFromImage(image) ;
                    uploadTexture(qim, mat, 0) ;
                }
            }
            if ( material->specularTexture() ) flags |= PhongMaterialProgram::HAS_SPECULAR_TEXTURE ;

            MaterialProgramPtr prog = PhongMaterialProgram::instance(flags) ;

            materials_.emplace(make_pair(mat, prog)) ;
        }

    }
}

Renderer::~Renderer() {

}

void Renderer::render(const xviz::CameraPtr &cam) {
    glEnable(GL_DEPTH_TEST) ;
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE) ;
    glCullFace(GL_BACK) ;
    glFrontFace(GL_CCW) ;

//    glEnable (GL_BLEND);
 //   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Vector4f bg_clr = cam->bgColor() ;

    glClearColor(bg_clr.x(), bg_clr.y(), bg_clr.z(), bg_clr.w()) ;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if ( const auto &pcam = dynamic_pointer_cast<xviz::PerspectiveCamera>(cam) ) {
        perspective_ = pcam->projectionMatrix() ;
        znear_ = pcam->zNear() ;
        zfar_ = pcam->zFar() ;
    }

    const xviz::Viewport &vp = cam->getViewport() ;
    glViewport(vp.x_, vp.y_, vp.width_, vp.height_);

    proj_ = cam->getViewMatrix() ;

    for ( const xviz::NodePtr &node: scene_->nodes() ) {
        if ( node->parent() == nullptr )
            render(node, Matrix4f::Identity()) ;
    }

}

void Renderer::uploadTexture(QImage im, xviz::MaterialPtr mat, int slot) {
    QOpenGLTexture *texture = new QOpenGLTexture(im);
    texture->setMinificationFilter(QOpenGLTexture::Linear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat) ;
    texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat) ;
    texture->generateMipMaps();

    textures_[mat.get()][slot] = texture ;
}


void Renderer::render(const xviz::NodePtr &node, const Matrix4f &tf) {

    Matrix4f mat = node->matrix().matrix(),
            tr = tf * mat ; // accumulate transform

    const auto &drawables = node->drawables() ;

    for( const auto &m: drawables )
        render(m.get(), tr) ;

    for( uint i=0 ; i<node->numChildren() ; i++ ) {
        const xviz::NodePtr &n = node->getChild(i) ;
        render(n, tr) ;
    }
}


#define MAX_LIGHTS 10

void Renderer::setLights(const xviz::NodePtr &node, const Affine3f &parent_tf, const MaterialProgramPtr &mat)
{
    Affine3f tf = parent_tf * node->matrix() ;

    xviz::LightPtr l = node->light() ;

    if ( light_index_ >= MAX_LIGHTS ) return ;

    if ( l ) {
        mat->applyLight(light_index_, l, tf) ;
        light_index_ ++ ;
    }

    for( uint i=0 ; i<node->numChildren() ; i++ )
        setLights(node->getChild(i), tf, mat) ;
}

void Renderer::setLights(const MaterialProgramPtr &material) {
    light_index_ = 0 ;

    Isometry3f mat ;
    mat.setIdentity() ;

    for( const xviz::NodePtr &node: scene_->nodes() )
        if ( node->parent() == nullptr )
            setLights(node, mat, material) ;
}


void Renderer::render(const xviz::Drawable *geom, const Matrix4f &mat)
{
    xviz::MeshPtr mesh = geom->geometry() ;
    if ( !mesh ) return ;

    auto it = meshes_.find(mesh.get()) ;
    if ( it == meshes_.end() ) return ;

    const MeshData &data = *it->second ;

    xviz::MaterialPtr material = geom->material() ;

    MaterialProgramPtr prog ;

    if ( material ) {
        auto mit = materials_.find(material) ;
        if ( mit == materials_.end() ) return ;
        prog = mit->second ;
    } else {
        prog = default_prog_ ;
        material = default_material_ ;
    }

    prog->use() ;
    prog->applyParams(material) ;
    prog->applyTransform(perspective_, proj_, mat) ;

    setLights(prog) ;

    auto tit = textures_.find(material.get()) ;
    if ( tit != textures_.end() ) {
        const TextureData &data = tit->second ;
        for( int i=0 ; i<4 ; i++ ) {
            QOpenGLTexture *texture = data[i] ;
            if ( texture != nullptr ) {
                texture->bind(i) ;
            }
        }
    }



 //   if ( mesh && mesh->hasSkeleton() )
 //       setPose(mesh, material) ;

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
    drawMeshData(data, mesh) ;

#endif
     glUseProgram(0) ;
}

void Renderer::drawMeshData(const MeshData &data, xviz::MeshPtr mesh) {

    data.vao_->bind() ;

    if ( mesh ) {
        if ( mesh->ptype() == xviz::Mesh::Triangles ) {
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
        else if ( mesh->ptype() == xviz::Mesh::Lines ) {
            glDrawArrays(GL_LINES, 0, data.elem_count_) ;
        }
        else if ( mesh->ptype() == xviz::Mesh::Points ) {
            glDrawArrays(GL_POINTS, 0, data.elem_count_) ;
        }

    } else {
        glDrawArrays(GL_TRIANGLES, 0, data.elem_count_) ;
    }

    data.vao_->release() ;

    glFlush();
}

