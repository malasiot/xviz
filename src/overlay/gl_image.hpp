#pragma once

#include <xviz/common/image.hpp>

#include "common/gl/gl3w.h"

namespace xviz { namespace impl {

class OpenGLImage {
public:
    OpenGLImage(const xviz::Image &im) ;

    void draw(float x, float y, float w, float h) ;

    int width() const { return width_ ; }
    int height() const { return height_ ; }

    ~OpenGLImage();
private:

    int width_, height_ ;
    GLuint texture_id_ ;
    bool generate_mipmaps_ = true ;

    GLuint vao_, vbo_, ebo_ ;

    static const GLuint POS_ATTRIBUTE = 0 ;
    static const GLuint UV_ATTRIBUTE = 1 ;

};




}}
