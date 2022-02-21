#ifndef XVIZ_RENDERER_HPP
#define XVIZ_RENDERER_HPP

#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/camera.hpp>
namespace xviz {

namespace impl {
class Renderer ;
}

class Renderer {
public:

    Renderer() ;
    ~Renderer() ;

    void init() ;

    // render a scene hierarchy

    void render(const NodePtr &scene, const CameraPtr &cam) ;

    void setDefaultFBO(unsigned int fbo) ;

    // transform model coordinates to screen coordinates
    Eigen::Vector2f project(const Eigen::Vector3f &pos) ;

private:

    std::unique_ptr<impl::Renderer> impl_ ;
} ;

}
#endif
