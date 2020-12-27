#ifndef XVIZ_QT_SCENE_SHADOW_MAP_RENDERER_HPP
#define XVIZ_QT_SCENE_SHADOW_MAP_RENDERER_HPP

#include <xviz/scene/light.hpp>
#include <xviz/scene/scene.hpp>
#include <xviz/qt/scene/mesh_data.hpp>

#include "shadow_map.hpp"

#include <QOpenGLShaderProgram>

class ShadowMapRenderer: QOpenGLFunctions_3_3_Core {
  public:
    ShadowMapRenderer(const std::map<const xviz::Geometry *, std::unique_ptr<MeshData>> &meshes);

    void setup(const xviz::Light *light) ;
    void setDefaultFBO(GLuint fbo) { default_fbo_ = fbo ; }

    void render(const xviz::Light *, const xviz::ScenePtr &scene);
    void render(const xviz::Light *, const Eigen::Matrix4f &mat, const xviz::NodePtr &node,
                const Eigen::Matrix4f &tr);
    void render(const xviz::Light *, const Eigen::Matrix4f &mat, const xviz::Drawable &dr,
                const Eigen::Matrix4f &tr);

    void drawMeshData(const MeshData &data, xviz::GeometryPtr mesh);

    void renderDebug() ;


private:
    QOpenGLShaderProgram shader_, debug_shader_ ;

    const std::map<const xviz::Geometry *, std::unique_ptr<MeshData>> &meshes_ ;
    GLuint default_fbo_ ;

    void renderQuad();
};


#endif
