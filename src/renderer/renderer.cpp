#include <xviz/scene/renderer.hpp>
#include "renderer_impl.hpp"

#include "shadow_map.hpp"
#include "util.hpp"
#include "mesh_data.hpp"
#include "material_program.hpp"
#include "texture_data.hpp"

#include <xviz/scene/scene.hpp>
#include <xviz/scene/node.hpp>
#include <xviz/scene/drawable.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/camera.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/material.hpp>

#include <iostream>

#include "mesh_data.hpp"

using namespace Eigen ;
using namespace std ;

namespace xviz {


namespace impl {

static bool gl_initialized_ = false ;

Renderer::Renderer() {
    PhongMaterial *mat = new PhongMaterial() ;
    mat->setDiffuseColor({0.5, 0.5, 0.5}) ;
    default_material_.reset(mat) ;

    init() ;

}

// lazy loading of textures on GPU and caching
impl::TextureData *Renderer::fetchTextureData(const Texture2D *texture) {
    if ( !texture ) return nullptr ;

    return textures_.fetch(texture->image().get(), texture->sampler()) ;

}

MaterialProgramPtr Renderer::instantiateMaterial(const Material *mat, const std::vector<LightData *> &lights, bool has_skeleton) {

    MaterialProgramParams params ;

    bool has_shadows = false ;

    for( const auto &ld: lights ) {
        const LightPtr &light = ld->light_ ;
        if ( light->castsShadows() ) has_shadows = true ;
        if ( dynamic_cast<const DirectionalLight *>(light.get()) ) {
            if ( light->castsShadows() ) params.num_dir_lights_shadow_ ++ ;
            else params.num_dir_lights_ ++ ;
        } else if ( dynamic_cast<const SpotLight *>(light.get()) ) {
            if ( light->castsShadows() ) params.num_spot_lights_shadow_ ++ ;
            else params.num_spot_lights_ ++ ;
        } else if ( dynamic_cast<const PointLight *>(light.get()) ) {
            if ( light->castsShadows() ) params.num_point_lights_shadow_ ++ ;
            else params.num_point_lights_ ++ ;
        }
    }

    params.enable_shadows_ = has_shadows ;
    params.enable_skinning_ = has_skeleton ;

    params.has_texture_map_ = mat->hasTexture() ;

    return mat->instantiate(params) ;
}

void Renderer::init() {
    if ( !gl_initialized_ ) {
        gl3wInit();
        gl_initialized_ = true ;
    }
}

Renderer::~Renderer() {

}

void Renderer::initState(const Material *mat) {

    glDepthFunc(GL_LEQUAL);

    glFrontFace(GL_CCW) ;
    glEnable (GL_BLEND);

    glEnable(GL_LINE_SMOOTH) ;
    glLineWidth(1.0) ;

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

    if ( mat->hasDepthTest() )
        glEnable(GL_DEPTH_TEST) ;
    else
        glDisable(GL_DEPTH_TEST) ;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::updateShadows(LightData &ld) {

    const auto light = ld.light_ ;

    if ( const DirectionalLight *dl = dynamic_cast<const DirectionalLight *>(light.get()) ) {
        Matrix4f lightProjection = dl->shadowCamera().getProjectionMatrix() ;
        Matrix4f lightView = lookAt(dl->position(), dl->target(), Vector3f(0.0, 1.0, 0.0));

        ld.ls_mat_ = lightProjection * lightView;

        renderShadowMap(ld);
    } else if ( const SpotLight *sl = dynamic_cast<const SpotLight *>(light.get()) ) {
        Matrix4f lightProjection = sl->shadowCamera().getProjectionMatrix() ;
        Matrix4f lightView = lookAt(sl->position(), {0, 0, 0}, Vector3f(0.0, 1.0, 0.0));

        ld.ls_mat_ = lightProjection * lightView;

        renderShadowMap(ld);
    }
}

void Renderer::renderShadowMap(const LightData &sd) {

    sd.shadow_map_->bind();

    glViewport(0, 0, shadow_map_width_, shadow_map_height_);

    sd.shadow_map_->bindTexture(GL_TEXTURE0) ;

    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    shadow_map_shader_->use() ;
    shadow_map_shader_->setUniform("lightSpaceMatrix", sd.ls_mat_);

    for ( const ConstNodePtr &node: scene_->getNodesRecursive() ) {
        for( const auto &dr: node->drawables() ) {
            GeometryPtr geom = dr.geometry() ;

            if ( !geom || !geom->castsShadows()) continue ;

            const MeshData *data = meshes_.fetch(geom.get()) ;

            if ( !data ) continue ;

            shadow_map_shader_->setUniform("model", node->globalTransform().matrix()) ;
            drawMeshData(*data, geom, true) ;
        }
    }

    sd.shadow_map_->unbind(default_fbo_) ;
}

void Renderer::render(const NodePtr &scene, const CameraPtr &cam, bool cb) {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &default_fbo_);

    scene_ = scene ;
    // render background

    meshes_.flush() ;

    Vector4f bg_clr = cam->bgColor() ;

    if ( cb ) {
        glClearColor(bg_clr.x(), bg_clr.y(), bg_clr.z(), bg_clr.w()) ;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }

    // setup camera matrices

    perspective_ = cam->getProjectionMatrix() ;

    proj_ = cam->getViewMatrix() ;

    renderScene(cam) ;

    //  glFlush() ;
}

void Renderer::renderText(const std::string &text, float x, float y, const Font &font, const Vector3f &clr) {
/*    if ( text.empty() ) return ;

    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );

    OpenGLText ti(text, font, clr) ;
    ti.render(x, viewport[3] - y) ;
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


void Renderer::renderShadowDebug(const LightData &sd) {

    shadow_map_debug_shader_->use() ;
    shadow_map_debug_shader_->setUniform("depthMap", 0);
    shadow_map_debug_shader_->setUniform("near_plane", -10.f) ;
    shadow_map_debug_shader_->setUniform("far_plane", 7.0f) ;

    sd.shadow_map_->bindTexture(GL_TEXTURE0) ;
    renderQuad() ;
}

void Renderer::renderScene(const CameraPtr &cam) {
    for ( ConstNodePtr node: scene_->getOrderedNodes() ) {
        for( const auto &drawable: node->drawables() ) {
            GeometryPtr mesh = drawable.geometry() ;
            if ( !mesh ) continue ;

            // prefetch fetch vbo
            meshes_.fetch(mesh.get()) ;

            if ( !node->isVisible() ) continue  ;
            render(cam, drawable, node->globalTransform() ) ;
        }
    }
}


LightData &Renderer::getLightData(const LightPtr &l) {

    LightData &data = light_data_[l] ;

    initShadowMapRenderer();

    if ( !data.shadow_map_ ) {
        data.shadow_map_.reset(new ShadowMap()) ;
        data.shadow_map_->init(shadow_map_width_, shadow_map_height_) ;
        data.shadow_map_->unbind(default_fbo_);
    }

    return data ;
}

std::vector<LightData *> Renderer::getLights() {
    std::vector<LightData *> lights ;
    for ( NodePtr &node: scene_->getNodesRecursive() ) {
        LightPtr l = node->light() ;
        if ( l ) {
            LightData &ld = getLightData(l) ;
            ld.light_ = l ;
            ld.mat_ = node->globalTransform() ;

            if ( l->castsShadows() ) {
               updateShadows(ld) ;
            }

            lights.push_back(&ld) ;
        }
    }

    return lights ;
}

void Renderer::render(const CameraPtr &cam, const Drawable &dr, const Affine3f &mat)
{
    GeometryPtr mesh = dr.geometry() ;
    if ( !mesh ) return ;

    // fetch vbo
    const MeshData *data = meshes_.fetch(mesh.get()) ;

    // get material
    MaterialPtr material = dr.material() ;

    if ( !material )
        material = default_material_ ;

    MaterialProgramPtr prog ;

    // collect lights and associated data

    std::vector<LightData *> lights = getLights() ;

    // create or load program for material
    prog = instantiateMaterial(material.get(), lights, mesh->hasSkeleton()) ;

    // init GL state

    initState(material.get()) ;

    // apply uniforms to material
    prog->use() ;
    prog->applyParams(material) ;
    prog->applyTransform(perspective_, proj_, mat.matrix()) ;
    prog->applyLights(lights) ;
    prog->bindTextures(material, [this](const Texture2D *t) {
        return fetchTextureData(t) ;
    }) ;

    if ( mesh && mesh->hasSkeleton() )
        setPose(mesh, prog) ;

    // do rendering

    const Viewport &vp = cam->getViewport() ;
    glViewport(vp.x_, vp.y_, vp.width_, vp.height_);

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
    drawMeshData(*data, mesh, false) ;

#endif

/*
    glBlendFunc(GL_ONE, GL_ZERO);
          glViewport(0, 0, 256, 256);
       //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
         renderShadowDebug(*lights[1]) ;
      glBindTexture(GL_TEXTURE_2D, 0);
*/
    glUseProgram(0) ;
}

void Renderer::initShadowMapRenderer() {
    if ( shadow_map_shader_ ) return ;

    shadow_map_shader_.reset(new OpenGLShaderProgram) ;
    shadow_map_shader_->addShaderFromFile(VERTEX_SHADER, "@shadow_map_shader_vs") ;
    shadow_map_shader_->addShaderFromFile(FRAGMENT_SHADER, "@shadow_map_shader_fs") ;
    shadow_map_shader_->link() ;

    shadow_map_debug_shader_.reset(new OpenGLShaderProgram) ;
    shadow_map_debug_shader_->addShaderFromFile(VERTEX_SHADER, "@shadow_debug_shader_vs") ;
    shadow_map_debug_shader_->addShaderFromFile(FRAGMENT_SHADER, "@shadow_debug_shader_fs") ;
    shadow_map_debug_shader_->link() ;
}


void Renderer::setPose(const GeometryPtr &mesh, const MaterialProgramPtr &mat) {
    const auto &skeleton = mesh->skeleton() ;
    for( unsigned int i=0 ; i<skeleton.size() ; i++ ) {
        const Geometry::Bone &b = skeleton[i] ;
        mat->applyBoneTransform(i, ( b.node_->globalTransform() * b.offset_).matrix()) ;
    }
}

void Renderer::drawMeshData(const MeshData &data, GeometryPtr mesh, bool solid) {

    glBindVertexArray(data.vao_);

    if ( mesh ) {
        if ( mesh->ptype() == Geometry::Triangles ) {
            if ( data.index_ ) {
                // bind index buffer if you want to render indexed data
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.index_);
                // indexed draw call
                glDrawElements(GL_TRIANGLES, data.indices_, GL_UNSIGNED_INT, nullptr);
            }
            else
                glDrawArrays(GL_TRIANGLES, 0, data.elem_count_) ;
        }
        else if ( mesh->ptype() == Geometry::Lines && !solid ) {
            if ( data.index_ ) {
                // bind index buffer if you want to render indexed data
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.index_);
                // indexed draw call
                glDrawElements(GL_LINES, data.indices_, GL_UNSIGNED_INT, nullptr);
            }
            else
                glDrawArrays(GL_LINES, 0, data.elem_count_) ;
        }
        else if ( mesh->ptype() == Geometry::Points ) {
            glDrawArrays(GL_POINTS, 0, data.elem_count_) ;
        }

    } else {
        glDrawArrays(GL_TRIANGLES, 0, data.elem_count_) ;
    }

    glBindVertexArray(0) ;
}

}

Renderer::Renderer(): impl_(new impl::Renderer()) {

}

Renderer::~Renderer() {}

void Renderer::init() {
    impl_->init() ;
}

void Renderer::render(const NodePtr &scene, const CameraPtr &cam, bool clear_buffers) {
    impl_->render(scene, cam, clear_buffers) ;
}

void Renderer::renderText(const string &text, float x, float y, const Font &font, const Vector3f &clr)
{
    impl_->renderText(text, x, y, font, clr) ;
}

Vector2f Renderer::project(const Vector3f &pos) {
    return impl_->project(pos) ;
}


}
