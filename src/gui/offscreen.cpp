#include <xviz/gui/offscreen.hpp>
#include <QOpenGLFunctions>
#include <QImage>

#include <xviz/scene/renderer.hpp>
#include <xviz/common/image.hpp>

#include <iostream>
using namespace std ;

namespace xviz {

OffscreenRenderer::OffscreenRenderer(const QSize &size): QOffscreenSurface(nullptr), size_(size) {
    QSurfaceFormat sformat;
    sformat.setDepthBufferSize(24);
    sformat.setMajorVersion(3);
    sformat.setMinorVersion(3);

    sformat.setSamples(4);
    sformat.setProfile(QSurfaceFormat::CoreProfile);


    setFormat(sformat);
    create();
    createContext() ;
    createFBO() ;
}



OffscreenRenderer::OffscreenRenderer(const QSize &size, QSurfaceFormat &sformat): QOffscreenSurface(nullptr), size_(size) {
    setFormat(sformat);
    create();

    createContext() ;
    createFBO() ;
}

void OffscreenRenderer::createFBO() {
    if ( context_ && fbo_ == nullptr ) {
        QOpenGLFramebufferObjectFormat format;
        format.setSamples(0);
        format.setTextureTarget(GL_TEXTURE_2D) ;
        format.setAttachment(QOpenGLFramebufferObject::Depth) ;


        fbo_ = new QOpenGLFramebufferObject(size_, format);
        fbo_->bind() ;

    }
}

void OffscreenRenderer::createContext() {
    context_ = new QOpenGLContext(this);
    context_->setFormat(format());

    if ( context_->create()) {
        context_->makeCurrent(this);
        context_->functions()->initializeOpenGLFunctions();
    } else {
        delete context_ ;
        context_ = Q_NULLPTR;
    }
}

OffscreenRenderer::~OffscreenRenderer() {
    delete context_ ;
    delete fbo_ ;
}

void OffscreenRenderer::render(const xviz::NodePtr &scene, const xviz::CameraPtr &cam)
{
    Renderer rdr ;
    rdr.init() ;
    rdr.render(scene, cam) ;

}

ImagePtr OffscreenRenderer::getImage() const {
    uchar *bytes = new uchar [size_.width() * size_.height() * 4] ;

    glReadPixels(0, 0, size_.width(), size_.height(), GL_RGBA, GL_UNSIGNED_BYTE, bytes);

    for(int line = 0; line != size_.height()/2; ++line) {
        std::swap_ranges(
                bytes + 4 * size_.width() * line,
                bytes + 4 * size_.width() * (line + 1),
                bytes + 4 * size_.width() * (size_.height() - line - 1));
    }

    return ImagePtr(new Image(bytes, ImageFormat::rgba32, size_.width(), size_.height())) ;
}

ImagePtr OffscreenRenderer::getDepthBuffer(float znear, float zfar) const {

    glReadBuffer(GL_DEPTH_ATTACHMENT);

    std::unique_ptr<float []> data(new float[size_.width() * size_.height() * sizeof(float)]) ;
    uint16_t *dst = new uint16_t[size_.width() * size_.height() * 2] ;

    glReadPixels(0, 0, size_.width(), size_.height(), GL_DEPTH_COMPONENT, GL_FLOAT, data.get());

    float max_allowed_z = zfar * 0.99;

    float *ptr = data.get() ;
    uint16_t *l_dst = dst + (size_.height() - 1) * size_.width();

    for (int i = 0; i < size_.height() ; ++i) {
        uint16_t *p_dst = l_dst ;
        for (int j = 0; j < size_.width(); ++j )
          {
              //need to undo the depth buffer mapping
              //http://olivers.posterous.com/linear-depth-in-glsl-for-real
              float z  = 2 * zfar * znear / (zfar + znear - (zfar - znear) * (2 * (*ptr) - 1));

              uint16_t val = ( z > max_allowed_z ) ? 0 : round(z * 1000) ;

              ++ptr ;
              *p_dst++ = val ;
          }
        l_dst -= size_.width() ;
    }

    return ImagePtr(new Image((uchar *)dst, ImageFormat::gray16, size_.width(), size_.height())) ;
}

}
