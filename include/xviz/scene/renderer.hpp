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

struct LightData {
    LightPtr light_ ;
    Eigen::Affine3f lmat_ ;
};

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

    Eigen::Matrix4f perspective_, proj_ ;

    MaterialPtr default_material_ ;

    float znear_, zfar_ ;

    unsigned int light_index_ = 0 ;

    int flags_ ;

    using TextureBundle = std::array<std::unique_ptr<impl::TextureData>, 4> ;

    std::map<GeometryPtr, std::unique_ptr<impl::MeshData>> meshes_ ;
    std::map<const Material *, impl::MaterialProgramPtr> materials_ ;
    std::map<const Material *, TextureBundle> textures_ ;
    unsigned int default_fbo_ ;

    std::unique_ptr<impl::OpenGLShaderProgram> shadow_map_shader_, shadow_map_debug_shader_ ;

    const uint32_t shadow_map_width_ = 2048 ;
    const uint32_t shadow_map_height_ = 2048 ;

    struct ShadowData {
        std::unique_ptr<impl::ShadowMap> shadow_map_ ;
        Eigen::Matrix4f ls_mat_ ;
    };

    std::map<LightPtr, ShadowData> shadow_data_ ;

    std::shared_ptr<ResourceLoader> resource_loader_ ;


private:


    void drawMeshData(const impl::MeshData &data, GeometryPtr mesh, bool solid=false);
    void setLights(const impl::MaterialProgramPtr &material);
    void setLights(const NodePtr &node, const Eigen::Affine3f &parent_tf, const impl::MaterialProgramPtr &mat);
    void setupTexture(const Material *mat, const Texture2D *texture, unsigned int slot);
    void setupCulling(const Material *mat);
    impl::MaterialProgramPtr instantiateMaterial(const Material *mat, const std::vector<LightData> &lights, int flags);
    void setPose(const GeometryPtr &mesh, const impl::MaterialProgramPtr &mat);
    void renderShadowMaps();
    void renderScene(const CameraPtr &cam);
    void render(const CameraPtr &cam, const Drawable &dr, const Eigen::Affine3f &mat);
    void initShadowMapRenderer() ;
    void renderShadowMap(const ShadowData &l);
    void setupShadows(const LightPtr &light);
    void renderShadowDebug(const ShadowData &sd);
    void renderQuad();
    void uploadTexture(impl::TextureData *data, const Material *material, int slot) ;
    std::vector<LightData> getLights() ;



    const impl::MeshData *fetchMeshData(GeometryPtr &geom);
} ;

}
#endif
