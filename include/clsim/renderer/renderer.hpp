#ifndef CVX_VIZ_RENDERER_HPP
#define CVX_VIZ_RENDERER_HPP

#include <memory>

#include <clsim/scene/scene.hpp>

namespace cvx { namespace viz {

namespace detail {
    class RendererImpl ;
}

class Renderer {
public:

    enum { RENDER_SHADOWS = 1 };

    Renderer(int flags = 0) ;
    ~Renderer() ;

    void setCamera(const CameraPtr &cam) ;

    void clearZBuffer();

//    cv::Mat getColor(bool alpha = true);
//    cv::Mat getColor(cv::Mat &bg, float alpha);
//    cv::Mat getDepth();

    // render a scene hierarchy

    void render(const ScenePtr &scene) ;

   void setDefaultFBO(uint fbo) ;
private:

    std::unique_ptr<detail::RendererImpl> impl_ ;
} ;


}}

#endif
