#ifndef XVIZ_QT_SCENE_SHADOW_MAP_HPP
#define XVIZ_QT_SCENE_SHADOW_MAP_HPP

#include <QOpenGLFunctions_3_3_Core>


class ShadowMap: QOpenGLFunctions_3_3_Core {
public:
    ShadowMap() ;
    ~ShadowMap();

    bool init(uint32_t width, uint32_t height);

    void bind();
    void unbind(GLuint default_fbo);

    void bindTexture(GLenum TextureUnit);

    bool ready() const { return fbo_ != 0 ; }

private:
    GLuint fbo_ = 0, texture_id_ = 0 ;
};

#endif
