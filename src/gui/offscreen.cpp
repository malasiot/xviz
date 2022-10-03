#include <xviz/gui/offscreen.hpp>
#include <QOpenGLFunctions>
#include <QImage>

#include <xviz/scene/renderer.hpp>

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

QImage OffscreenRenderer::getImage() const {
    return fbo_->toImage() ;
}

QImage OffscreenRenderer::getDepthBuffer(float znear, float zfar) const {

    QImage im(size_, QImage::Format_RGB888 ) ;

    glReadBuffer(GL_DEPTH_ATTACHMENT);

    std::unique_ptr<float []> data(new float[size_.width() * size_.height() * sizeof(float)]) ;
    glReadPixels(0, 0, size_.width(), size_.height(), GL_DEPTH_COMPONENT, GL_FLOAT, data.get());

    float max_allowed_z = zfar * 0.99;

      //unsigned int i_min = width_, i_max = 0, j_min = height_, j_max = 0;

    float *ptr = data.get() ;
    uchar *dst = im.bits() ;

    for (int i = 0; i < size_.height() ; ++i) {
        for (int j = 0; j < size_.width(); ++j )
          {
              //need to undo the depth buffer mapping
              //http://olivers.posterous.com/linear-depth-in-glsl-for-real
              float z  = 2 * zfar * znear / (zfar + znear - (zfar - znear) * (2 * (*ptr) - 1));

              uint64_t val = ( z > max_allowed_z ) ? 0 : round(z * 1e9) ;

//              cout << z << endl ;
              QColor clr ;

              uchar r = (val >> 16) & 0xFF;
              uchar g = (val >> 8) & 0xFF;
              uchar b = val & 0xFF;

              ++ptr ;
              *dst++ = r ;
              *dst++ = g ;
              *dst++ = b ;
          }
    }

      return im.mirrored(false, true);
}

}
