#ifndef XVIZ_RENDERER_HPP
#define XVIZ_RENDERER_HPP

#include <map>
#include <Eigen/Geometry>
#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/material.hpp>
#include <xviz/common/resource_loader.hpp>

namespace xviz {

namespace impl {
class MeshData ;
class ShadowMap ;
class OpenGLShaderProgram ;
class MaterialProgram ;
class TextureData ;
using MaterialProgramPtr = std::shared_ptr<MaterialProgram> ;
}

class Renderer {
public:

    enum { RENDER_SHADOWS = 1 };

    Renderer(int flags = 0) ;
    ~Renderer() ;

    void init(const NodePtr &scene) ;

    void clearZBuffer();

    // render a scene hierarchy

    void render(const CameraPtr &scene) ;

    void setDefaultFBO(unsigned int fbo) {
        default_fbo_ = fbo ;
    }

    // transform model coordinates to screen coordinates
    Eigen::Vector2f project(const Eigen::Vector3f &pos) ;


private:

    NodePtr scene_ ;

    Eigen::Matrix4f perspective_, proj_, ls_mat_ ;

    MaterialPtr default_material_ ;

    float znear_, zfar_ ;

    unsigned int light_index_ = 0 ;

    int flags_ ;

    using TextureBundle = std::array<std::unique_ptr<impl::TextureData>, 4> ;

    std::map<const Geometry *, std::unique_ptr<impl::MeshData>> meshes_ ;
    std::map<const Material *, impl::MaterialProgramPtr> materials_ ;
    std::map<const Material *, TextureBundle> textures_ ;
    unsigned int default_fbo_ ;

    std::unique_ptr<impl::OpenGLShaderProgram> shadow_map_shader_, shadow_map_debug_shader_ ;
    std::unique_ptr<impl::ShadowMap> shadow_map_ ;

    const uint32_t shadow_map_width_ = 1024 ;
    const uint32_t shadow_map_height_ = 1024 ;

    std::shared_ptr<ResourceLoader> resource_loader_ ;


private:
    void render(const NodePtr &node, const Eigen::Matrix4f &tf);
    void render(const Drawable &geom, const Eigen::Matrix4f &mat);
    void drawMeshData(const impl::MeshData &data, GeometryPtr mesh, bool solid=false);
    void setLights(const impl::MaterialProgramPtr &material);
    void setLights(const NodePtr &node, const Eigen::Affine3f &parent_tf, const impl::MaterialProgramPtr &mat);
    void setupTexture(const Material *mat, const Texture2D *texture, unsigned int slot);
    void setupCulling(const Material *mat);
    impl::MaterialProgramPtr instantiateMaterial(const Material *mat, int flags);
    void setPose(const GeometryPtr &mesh, const impl::MaterialProgramPtr &mat);
    void renderShadowMaps();
    void renderScene(const LightPtr &l, const Eigen::Affine3f &light_mat);
    void render(const Drawable &dr, const Eigen::Affine3f &mat, const LightPtr &l, const Eigen::Affine3f &lmat);
    void initShadowMapRenderer() ;
    void renderShadowMap(const LightPtr &l);
    void setupShadows(const LightPtr &light);
    void renderShadowDebug();
    void renderQuad();
    void uploadTexture(impl::TextureData *data, const Material *material, int slot) ;


    const impl::MeshData *fetchMeshData(GeometryPtr &geom);
} ;

}
#endif
