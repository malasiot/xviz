#include <xviz/qt/scene/offscreen.hpp>
#include <QOpenGLFunctions>
#include <QImage>

#include <xviz/qt/scene/renderer.hpp>

QOffscreenRenderer::QOffscreenRenderer(QScreen *targetScreen, const QSize &size): QOffscreenSurface(targetScreen),
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


QOffscreenRenderer::~QOffscreenRenderer() {
    if ( fbo_ ) delete fbo_ ;
}

QImage QOffscreenRenderer::render(const xviz::ScenePtr &scene, const xviz::CameraPtr &cam)
{
    Renderer rdr ;
    rdr.init(scene) ;
    rdr.render(cam) ;
    return fbo_->toImage() ;
}
