#pragma once

#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/renderer.hpp>
#include <xviz/gui/trackball.hpp>

#include <GLFW/glfw3.h>

namespace xviz {

class SceneViewerGLFW {
public:
    enum UpAxis { XAxis, YAxis, ZAxis } ;

    SceneViewerGLFW(const NodePtr &scene)  ;
    ~SceneViewerGLFW() ;

    static void initDefaultGLContext();

    void setScene(const NodePtr &s) ;

    void setDefaultCamera() ;

    void makeAxes(float r) ;

    void setDrawAxes(bool draw_axes);

    // should be called to initialized camera and trackball with given scene center and radius

    void initCamera(const Eigen::Vector3f &c, float r, UpAxis upAxis = YAxis );

    void startAnimations() ;

    CameraPtr getCamera() const { return camera_ ; }

    void runLoop() ;

    virtual void onMouseButtonPressed(uint button, size_t x, size_t y, uint flags) ;
    virtual void onMouseButtonReleased(uint button, size_t x, size_t y, uint flags) ;

    virtual void onKeyPressed(size_t key_code, uint mods) ;
    virtual void onKeyReleased(size_t key_code, uint mods) {}

    virtual void onMouseMoved(double xpos, double ypos) ;
    virtual void onMouseWheel(double x) ;
    virtual void onResize(int width, int height) ;

private:

    bool createWindow() ;

    void update() ;

    static void buttonCallback(GLFWwindow *window, int button, int action, int mods);
    static void errorCallback(int error, const char* description);
    static void keyCallback(GLFWwindow *window, int key, int scancode,
                            int action, int mods);
    static void moveCallback(GLFWwindow *window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow *window, double xpos, double ypos);
    static void sizeCallback(GLFWwindow *window, int width, int height);


protected:

    virtual void onUpdate(float delta) {}

    //void initializeGL() override;
    //void paintGL() override;
    //void resizeGL(int width, int height) override;

    NodePtr scene_, axes_ ;
    CameraPtr camera_ ;

    Renderer rdr_ ;
    TrackBall trackball_ ;

    bool draw_axes_ = false ;
    UpAxis axis_ = YAxis ;
    float aradius_, radius_ ;
    float ts_ = 0 ;

    GLFWwindow *handle_ ;
    size_t width_ = 512, height_ = 512 ;
    double saved_time_ = 0 ;

};


}
