#ifndef XVIZ_TEXT_HPP
#define XVIZ_TEXT_HPP

#include <memory>
#include <Eigen/Core>

#include <xviz/common/font.hpp>

namespace xviz {

namespace impl {
    class OpenGLText ;
    class RendererImpl ;
}

// wrapper for text object, usefull for repetitive drawing of the same text
class Text {
public:

    Text(const std::string &t, const Font &f) ;

    void render(float x, float y, const Eigen::Vector3f &clr) ;

private:

    friend class impl::RendererImpl ;
    std::shared_ptr<impl::OpenGLText> impl_ ;
} ;

}

#endif
