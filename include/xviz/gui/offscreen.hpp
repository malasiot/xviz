#ifndef XVIZ_QT_OFFSCREEN_HPP
#define XVIZ_QT_OFFSCREEN_HPP

#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>
#include <QOpenGLContext>

#include <xviz/scene/scene_fwd.hpp>
#include <xviz/common/image.hpp>

namespace xviz {
class OffscreenSurface : public QOffscreenSurface
{
    Q_OBJECT

public:

    explicit OffscreenSurface(const QSize& size);
    explicit OffscreenSurface(const QSize &size, QSurfaceFormat &sformat);

    ~OffscreenSurface();

    Image getImage() const ;
    Image getDepthBuffer(float znear, float zfar) const ;

private:
    QOpenGLFramebufferObject *fbo_ = nullptr;
    QOpenGLContext *context_ = nullptr;
    QSize size_;
private:
    void createContext();
    void createFBO();
    Image readPixels(QOpenGLFramebufferObject *fbo) const;
};

}

#endif
