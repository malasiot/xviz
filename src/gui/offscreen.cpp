#include <xviz/gui/offscreen.hpp>
#include <QOpenGLFunctions>
#include <QImage>

#include <xviz/scene/renderer.hpp>

namespace xviz {

OffscreenRenderer::OffscreenRenderer(QScreen *targetScreen, const QSize &size): QOffscreenSurface(targetScreen),
    size_(size)
{
    QSurfaceFormat sformat;
      sformat.setDepthBufferSize(24);
      sformat.setMajorVersion(3);
      sformat.setMinorVersion(3);

      sformat.setSamples(4);
      sformat.setProfile(QSurfaceFormat::CoreProfile);


    setFormat(sformat);
    create();


    context_ = new QOpenGLContext(this);
    context_->setFormat(format());

    if ( context_->create()) {
       context_->makeCurrent(this);
       context_->functions()->initializeOpenGLFunctions();
    } else {
        delete context_ ;
        context_ = Q_NULLPTR;
    }

    fbo_ = new QOpenGLFramebufferObject(size_, GL_TEXTURE_2D);
    fbo_->bind() ;
}


OffscreenRenderer::~OffscreenRenderer() {
    delete context_ ;
    delete fbo_ ;
}

QImage OffscreenRenderer::render(const xviz::NodePtr &scene, const xviz::CameraPtr &cam)
{
    Renderer rdr ;
    rdr.init() ;
    rdr.render(scene, cam) ;
    return fbo_->toImage() ;
}

}
