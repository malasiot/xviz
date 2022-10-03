#ifndef XVIZ_QT_OFFSCREEN_HPP
#define XVIZ_QT_OFFSCREEN_HPP

#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>
#include <QOpenGLContext>

#include <xviz/scene/scene_fwd.hpp>
#include <xviz/common/image.hpp>

namespace xviz {
class OffscreenRenderer : public QOffscreenSurface
{
    Q_OBJECT

public:

    explicit OffscreenRenderer(const QSize& size);
    explicit OffscreenRenderer(const QSize &size, QSurfaceFormat &sformat);

    ~OffscreenRenderer();

    void render(const xviz::NodePtr &scene, const xviz::CameraPtr &cam);

    ImagePtr getImage() const ;
    ImagePtr getDepthBuffer(float znear, float zfar) const ;


private:
    QOpenGLFramebufferObject *fbo_ = nullptr;
    QOpenGLContext *context_ = nullptr;
    QSize size_;
private:
    void createContext();
    void createFBO();
};

}

#endif
