#pragma once

#include <Eigen/Core>

#include "common/gl/gl3w.h"
#include "common/shader.hpp"

namespace xviz { namespace impl {


class OpenGLRect {
public:
    OpenGLRect() ;

    ~OpenGLRect() ;

    void render(float x, float y, float w, float h, const Eigen::Vector3f &fill_clr, const Eigen::Vector3f &border_clr,
                float border_width, float opacity) ;

private:


    GLuint vao_, vbo_, ebo_ ;
    Eigen::Vector3f fill_clr_, border_clr_ ;
    float border_width_ ;

    static const GLuint POS_ATTRIBUTE = 0 ;
    static const GLuint UV_ATTRIBUTE = 1 ;
};


}}
