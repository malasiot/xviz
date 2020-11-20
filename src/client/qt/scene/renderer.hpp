#ifndef XVIZ_QT_SCENE_RENDERER_HPP
#define XVIZ_QT_SCENE_RENDERER_HPP

#include <map>
#include <Eigen/Geometry>
#include <xviz/scene/scene_fwd.hpp>

#include <QOpenGLFunctions>

#include "material.hpp"

class MeshData ;

class Renderer: protected QOpenGLFunctions {
public:

    enum { RENDER_SHADOWS = 1 };

    Renderer(int flags = 0) ;
    ~Renderer() ;

    void init(const xviz::ScenePtr &scene) ;

    void clearZBuffer();

    // render a scene hierarchy

    void render(const xviz::CameraPtr &scene) ;
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

    xviz::CameraPtr cam_ ;
    xviz::ScenePtr scene_ ;

    Eigen::Matrix4f perspective_, proj_, ls_mat_ ;

    float znear_, zfar_ ;
//    MaterialInstancePtr default_material_ ;

    uint light_index_ = 0 ;

    //OpenGLShaderProgram::Ptr line_shader_ ;
    GLuint line_vao_, line_vbo_, line_idx_vbo_ ;
    GLint line_width_range_[2] ;

    int flags_ ;

    std::map<xviz::Mesh *, MeshData> meshes_ ;
    std::map<xviz::MaterialPtr, MaterialProgramPtr> materials_ ;
    std::vector<MaterialProgramPtr> programs_ ;

private:
    void render(const xviz::NodePtr &node, const Eigen::Matrix4f &tf);
    void render(const xviz::Drawable *geom, const Eigen::Matrix4f &mat);
    void drawMeshData(const MeshData &data, xviz::MeshPtr mesh);
} ;

#endif
