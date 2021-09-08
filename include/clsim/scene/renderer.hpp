#ifndef CLSIM_RENDERER_HPP
#define CLSIM_RENDERER_HPP

#include <map>
#include <Eigen/Geometry>
#include <clsim/scene/scene_fwd.hpp>
#include <clsim/scene/material.hpp>
#include <clsim/common/resource_loader.hpp>

namespace clsim {

namespace impl {
class MeshData ;
class ShadowMap ;
class OpenGLShaderProgram ;
class MaterialProgram ;
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

    void setDefaultFBO(uint fbo) {
        default_fbo_ = fbo ;
    }

    // transform model coordinates to screen coordinates
    Eigen::Vector2f project(const Eigen::Vector3f &pos) ;

    void uploadTexture(const Image &im, const Material *material, int slot) ;

private:

    NodePtr scene_ ;

    Eigen::Matrix4f perspective_, proj_, ls_mat_ ;

    MaterialPtr default_material_ ;

    float znear_, zfar_ ;

    uint light_index_ = 0 ;

    int flags_ ;

    using TextureData = std::array<uint, 4> ;

    std::map<const Geometry *, std::unique_ptr<impl::MeshData>> meshes_ ;
    std::map<const Material *, impl::MaterialProgramPtr> materials_ ;
    std::vector<impl::MaterialProgramPtr> programs_ ;
    std::map<const Material *, TextureData> textures_ ;
    uint default_fbo_ ;

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
    void setupTexture(const Material *mat, const Texture2D *texture, uint slot);
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

    const impl::MeshData *fetchMeshData(GeometryPtr &geom);
} ;

}
#endif
