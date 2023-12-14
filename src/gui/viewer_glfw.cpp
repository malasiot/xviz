#include <xviz/gui/viewer_glfw.hpp>
#include <xviz/scene/node.hpp>
#include <thread>

using namespace std ;
using namespace Eigen ;

namespace xviz {

SceneViewerGLFW::SceneViewerGLFW(const NodePtr &scene): scene_(scene) {
    createWindow() ;
}


bool SceneViewerGLFW::createWindow() {
    glfwSetErrorCallback(errorCallback);

    if( !glfwInit() ) return false ;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE) ;
  //  glfwWindowHint(GLFW_SAMPLES, 4);

    if ( !( handle_ = glfwCreateWindow(width_, height_, "GL", 0, 0) )) {
        glfwTerminate();
        return false ;
    }

    glfwSetWindowUserPointer(handle_, this) ;

    glfwMakeContextCurrent(handle_);
    glfwSwapInterval(1);
    glfwSetTime( 0.0 );

    glfwSetCursorPosCallback(handle_, moveCallback);
    glfwSetKeyCallback(handle_, keyCallback);
    glfwSetMouseButtonCallback(handle_, buttonCallback);
    glfwSetScrollCallback(handle_, scrollCallback);
    glfwSetFramebufferSizeCallback(handle_, sizeCallback);



/*
    while (!glfwWindowShouldClose(handle_))  {
        update() ;
        glfwPollEvents();
    }

    glfwDestroyWindow(handle_);

    glfwTerminate();

    return true ;
*/

    return true ;
}

void SceneViewerGLFW::runLoop() {
    assert(camera_ && "Camera not initialized");

    sizeCallback(handle_, width_, height_); // Set initial size.

    while (!glfwWindowShouldClose(handle_))  {
        update() ;
        glfwPollEvents();

    }

    glfwDestroyWindow(handle_);

    glfwTerminate();
}

void SceneViewerGLFW::onResize(int w, int h)
{
    if ( camera_ ) {
        float ratio = w/(float)h  ;
        std::static_pointer_cast<PerspectiveCamera>(camera_)->setAspectRatio(ratio) ;

        trackball_.setScreenSize(w, h);
        camera_->setViewport(w, h) ;

    }

    update() ;

}

void SceneViewerGLFW::update() {
    double current_time =  glfwGetTime();
    double elapsed_time = current_time - saved_time_;

 /*   if ( elapsed_time >= 1.0/60 ) {
        onUpdate(elapsed_time) ;

        saved_time_ = current_time ;
    }
    else this_thread::sleep_for(std::chrono::milliseconds(100)) ;
*/
    if ( !scene_ ) return ;

    rdr_.render(scene_, camera_) ;

    glfwSwapBuffers(handle_);
}

void SceneViewerGLFW::buttonCallback(GLFWwindow *window, int button, int action, int mods) {

    SceneViewerGLFW *instance = (SceneViewerGLFW *)glfwGetWindowUserPointer(window) ;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    switch(action)
    {
    case GLFW_PRESS:
        instance->onMouseButtonPressed(button, xpos, ypos, mods) ;
        break ;
    case GLFW_RELEASE:
        instance->onMouseButtonReleased(button, xpos, ypos, mods) ;
        break ;
    default: break;
    }
}

void SceneViewerGLFW::errorCallback(int error, const char *description) {

}

void SceneViewerGLFW::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    SceneViewerGLFW *instance = (SceneViewerGLFW *)glfwGetWindowUserPointer(window) ;

    switch(action) {
    case GLFW_PRESS:
        switch(key)
        {
        case GLFW_KEY_ESCAPE:
            // Exit app on ESC key.
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        default:
            instance->onKeyPressed(key, mods);
        }
        break;
    case GLFW_RELEASE:
        instance->onKeyReleased(key, mods) ;
    default: break;
    }

}

void SceneViewerGLFW::moveCallback(GLFWwindow *window, double xpos, double ypos) {
    SceneViewerGLFW *instance = (SceneViewerGLFW *)glfwGetWindowUserPointer(window) ;

    instance->onMouseMoved(xpos, ypos);
}

void SceneViewerGLFW::scrollCallback(GLFWwindow *window, double xpos, double ypos) {
    SceneViewerGLFW *instance = (SceneViewerGLFW *)glfwGetWindowUserPointer(window) ;
    instance->onMouseWheel(xpos + ypos);
}

void SceneViewerGLFW::sizeCallback(GLFWwindow *window, int width, int height) {
    SceneViewerGLFW *instance = (SceneViewerGLFW *)glfwGetWindowUserPointer(window) ;
    instance->onResize(width, height) ;
}

void SceneViewerGLFW::setDefaultCamera() {
    assert(scene_) ;
    auto c = scene_->geomCenter() ;
    auto r = scene_->geomRadius(c) ;
    initCamera(c, r, UpAxis::YAxis) ;
}

void SceneViewerGLFW::setDrawAxes(bool draw_axes) { draw_axes_ = draw_axes ; update() ; }

void SceneViewerGLFW::initCamera(const Vector3f &c, float r, UpAxis axis) {
    camera_.reset(new PerspectiveCamera(1.0, 70*M_PI/180, 0.01*r, 100*r)) ;

    axis_ = axis ;

    radius_ = r ;
    aradius_ = 10 * r ;

    if ( axis == YAxis )
        trackball_.setCamera(camera_, c + Vector3f{0.0, 0, 4*r}, c, {0, 1, 0}) ;
    else if ( axis == XAxis )
        trackball_.setCamera(camera_, c + Vector3f{0.0, 0, 4*r}, c, {1, 0, 0}) ;
    else if ( axis == ZAxis )
        trackball_.setCamera(camera_, c + Vector3f{0.0, 4*r, 0.0}, c, {0, 0, 1});

    trackball_.setZoomScale(0.1*r) ;

    camera_->setBgColor({1, 1, 1, 1}) ;

    camera_->setViewport(width_, height_) ;

}

void SceneViewerGLFW::onMouseButtonPressed(uint button, size_t x, size_t y, uint flags)
{
    if ( !camera_ ) return ;

    switch ( button ) {
    case GLFW_MOUSE_BUTTON_LEFT:
        trackball_.setLeftClicked(true) ;
        break ;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        trackball_.setMiddleClicked(true) ;
        break ;
    case GLFW_MOUSE_BUTTON_RIGHT:
        trackball_.setRightClicked(true) ;
        break ;
    default: break ;
    }
    trackball_.setClickPoint(x, y) ;
    trackball_.update() ;

  //  update() ;
}

void SceneViewerGLFW::onMouseButtonReleased(uint button, size_t x, size_t y, uint flags)
{
    if ( !camera_ ) return ;

    switch ( button ) {
    case GLFW_MOUSE_BUTTON_LEFT:
        trackball_.setLeftClicked(false) ;
        break ;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        trackball_.setMiddleClicked(false) ;
        break ;
    case GLFW_MOUSE_BUTTON_RIGHT:
        trackball_.setRightClicked(false) ;
        break ;
    default:
        break ;
    }
    trackball_.setClickPoint(x, y) ;
    trackball_.update() ;
 //   update() ;

}

void SceneViewerGLFW::onMouseMoved(double x, double y)
{
    if ( !camera_ ) return ;

    trackball_.setClickPoint(x, y) ;
    trackball_.update() ;

 //   update() ;

}

void SceneViewerGLFW::onMouseWheel(double e) {
    if ( !camera_ ) return ;

    trackball_.setScrollDirection(e>0);
    trackball_.update() ;
//    update() ;
}

void SceneViewerGLFW::onKeyPressed(size_t key, uint mods)
{
    if ( key == GLFW_KEY_A) {
        draw_axes_ = !draw_axes_ ;
        setDrawAxes(draw_axes_);
        update() ;
    } else if ( key == GLFW_KEY_ESCAPE ) {
         glfwSetWindowShouldClose(handle_, GL_TRUE);

    } else if ( key == GLFW_KEY_R ) {
  //      startRecording();
    } else if ( key == GLFW_KEY_S ) {
//        stopRecording() ;
    }

}
}
