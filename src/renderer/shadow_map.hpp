#ifndef XVIZ_RENDERER_SHADOW_MAP_HPP
#define XVIZ_RENDERER_SHADOW_MAP_HPP

#include "common/gl/gl3w.h"

namespace xviz { namespace impl {

class ShadowMap {
public:
    ShadowMap() ;
    ~ShadowMap();

    bool init(GLuint width, GLuint height);

    void bind();
    void unbind(GLuint default_fbo);

    void bindTexture(GLenum TextureUnit);

    bool ready() const { return fbo_ != 0 ; }

private:
    GLuint fbo_ = 0, texture_id_ = 0 ;
};

}}

#endif
