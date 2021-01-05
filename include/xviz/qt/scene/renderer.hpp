#ifndef XVIZ_QT_SCENE_RENDERER_HPP
#define XVIZ_QT_SCENE_RENDERER_HPP

#include <map>
#include <Eigen/Geometry>
#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/material.hpp>

#include <QOpenGLFunctions_3_3_Core>

#include "material.hpp"


class MeshData ;
class QOpenGLTexture ;
class ShadowMap ;

class Renderer: public QObject, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT
public:

    enum { RENDER_SHADOWS = 1 };

    Renderer(int flags = 0) ;
    ~Renderer() ;

    void init(const xviz::ScenePtr &scene) ;

    void clearZBuffer();

    // render a scene hierarchy

    void render(const xviz::CameraPtr &scene) ;

    void setDefaultFBO(GLuint fbo) {
        default_fbo_ = fbo ;
    }

    // transform model coordinates to screne coordinates
    Eigen::Vector2f project(const Eigen::Vector3f &pos) ;

private slots:

    void uploadTexture(QImage im, const xviz::Material *material, int slot) ;
#if 0
    // Draws text on top of the scene using given font and color
    void text(const std::string &text, float x, float y, const Font &f, const Eigen::Vector3f &clr) ;
    void text(const std::string &text, const Eigen::Vector3f &pos, const Font &f, const Eigen::Vector3f &clr) ;


    // It returns a text object that may be cached and drawn several times by calling render function.
    // It uses OpenGL so it should be called after initializing GL context
    Text textObject(const std::string &text, const Font &f) ;

    // Draws text object on top of the scene using given font and color
    void text(const Text &text, float x, float y, const Font &f, const Eigen::Vector3f &clr) ;
    void text(const Text &text, const Eigen::Vector3f &pos, const Font &f, const Eigen::Vector3f &clr) ;

    // draw a line with given color and width. suitable for drawing a few lines for debug purpose
    void line(const Eigen::Vector3f &from, const Eigen::Vector3f &to, const Eigen::Vector4f &clr, float lineWidth = 1.);

    // draw a 3D elliptic arc with given center and normal
    void arc(const Eigen::Vector3f &center, const Eigen::Vector3f &normal, const Eigen::Vector3f &xaxis,
             float radiusA, float radiusB,
             float minAngle, float maxAngle,
             const Eigen::Vector4f &color, bool drawSect, float lineWidth = 1.f, float stepDegrees = 10.f);

    void circle(const Eigen::Vector3f &center, const Eigen::Vector3f &normal, float radius, const Eigen::Vector4f &color, float lineWidth = 1.0) ;
#endif

private:


    xviz::ScenePtr scene_ ;

    Eigen::Matrix4f perspective_, proj_, ls_mat_ ;


    xviz::MaterialPtr default_material_ ;

    float znear_, zfar_ ;

    uint light_index_ = 0 ;

    int flags_ ;

    using TextureData = std::array<QOpenGLTexture *, 4> ;

    std::map<const xviz::Geometry *, std::unique_ptr<MeshData>> meshes_ ;
    std::map<const xviz::Material *, MaterialProgramPtr> materials_ ;
    std::vector<MaterialProgramPtr> programs_ ;
    std::map<const xviz::Material *, TextureData> textures_ ;
    GLuint default_fbo_ ;

    QOpenGLShaderProgram shadow_map_shader_, shadow_map_debug_shader_ ;
    std::unique_ptr<ShadowMap> shadow_map_ ;

    const uint32_t shadow_map_width_ = 1024 ;
    const uint32_t shadow_map_height_ = 1024 ;


private:
    void render(const xviz::NodePtr &node, const Eigen::Matrix4f &tf);
    void render(const xviz::Drawable &geom, const Eigen::Matrix4f &mat);
    void drawMeshData(const MeshData &data, xviz::GeometryPtr mesh, bool solid=false);
    void setLights(const MaterialProgramPtr &material);
    void setLights(const xviz::NodePtr &node, const Eigen::Affine3f &parent_tf, const MaterialProgramPtr &mat);
    void setupTexture(const xviz::Material *mat, const xviz::Texture2D *texture, uint slot);
    void setupCulling(const xviz::Material *mat);
    MaterialProgramPtr instantiateMaterial(const xviz::Material *mat, int flags);
    void setPose(const xviz::GeometryPtr &mesh, const MaterialProgramPtr &mat);
    void renderShadowMaps();
    void renderScene(const xviz::LightPtr &l, const Eigen::Affine3f &light_mat);
    void render(const xviz::Drawable &dr, const Eigen::Affine3f &mat, const xviz::LightPtr &l, const Eigen::Affine3f &lmat);
    void initShadowMapRenderer() ;
    void renderShadowMap(const xviz::LightPtr &l);
    void setupShadows(const xviz::LightPtr &light);
    void renderShadowDebug();
    void renderQuad();

    const MeshData *fetchMeshData(xviz::GeometryPtr &geom);
} ;

#endif
