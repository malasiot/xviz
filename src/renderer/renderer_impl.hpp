#ifndef XVIZ_RENDERER_IMPL_HPP
#define XVIZ_RENDERER_IMPL_HPP

#include <map>
#include <Eigen/Geometry>
#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/material.hpp>
#include <xviz/common/resource_loader.hpp>

#include "mesh_data.hpp"
#include "texture_data.hpp"
#include "material_program.hpp"

#include <iostream>

namespace xviz {
namespace impl {


class ShadowMap ;
class OpenGLShaderProgram ;
class MaterialProgram ;
class TextureCache ;

using MaterialProgramPtr = std::shared_ptr<MaterialProgram> ;

struct LightData {
    LightPtr light_ ;
    Eigen::Affine3f mat_ ;
    std::unique_ptr<impl::ShadowMap> shadow_map_ ;
    Eigen::Matrix4f ls_mat_ ;
};

class Renderer {
public:

    Renderer() ;
    ~Renderer() ;

    void init() ;

    void clearZBuffer();

    // render a scene hierarchy

    void render(const NodePtr &scene, const CameraPtr &cam) ;

    // transform model coordinates to screen coordinates
    Eigen::Vector2f project(const Eigen::Vector3f &pos) ;

private:

    NodePtr scene_;

    Eigen::Matrix4f perspective_, proj_ ;

    MaterialPtr default_material_ ;

    MeshDataManager meshes_ ;
    TextureCache textures_ ;
    GLint default_fbo_ ;

    std::unique_ptr<impl::OpenGLShaderProgram> shadow_map_shader_, shadow_map_debug_shader_ ;

    const uint32_t shadow_map_width_ = 2048 ;
    const uint32_t shadow_map_height_ = 2048 ;

    std::map<LightPtr, LightData> light_data_ ;
    MaterialProgramManager material_manager_ ;

private:


    void drawMeshData(const impl::MeshData &data, GeometryPtr mesh, bool solid=false);
    void setLights(const impl::MaterialProgramPtr &material);
    void setLights(const NodePtr &node, const Eigen::Affine3f &parent_tf, const impl::MaterialProgramPtr &mat);
    void setupTexture(const Material *mat, const Texture2D *texture, unsigned int slot);
    void initState(const Material *mat);
    impl::MaterialProgramPtr instantiateMaterial(const Material *mat, const std::vector<LightData *> &lights, bool skinning);
    void setPose(const GeometryPtr &mesh, const impl::MaterialProgramPtr &mat);

    void renderScene(const CameraPtr &cam);
    void render(const CameraPtr &cam, const Drawable &dr, const Eigen::Affine3f &mat);
    void initShadowMapRenderer() ;
    void renderShadowMap(const LightData &l);
    void updateShadows(LightData &light);
    void renderShadowDebug(const LightData &sd);
    void renderQuad();
    impl::TextureData *fetchTextureData(const Texture2D *tex) ;
    void uploadTexture(impl::TextureData *data, const Material *material, int slot) ;
    LightData &getLightData(const LightPtr &light) ;
    std::vector<LightData *> getLights();
} ;




}
} // namespace xviz
#endif
