#ifndef XVIZ_QT_OFFSCREEN_HPP
#define XVIZ_QT_OFFSCREEN_HPP

#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>
#include <QOpenGLContext>

#include <xviz/scene/scene_fwd.hpp>

namespace xviz {
class OffscreenRenderer : public QOffscreenSurface
{
    Q_OBJECT

public:

    explicit OffscreenRenderer(QScreen* targetScreen = nullptr, const QSize& size = QSize (1, 1));
    ~OffscreenRenderer();

    QImage render(const xviz::NodePtr &scene, const xviz::CameraPtr &cam);

private:
    QOpenGLFramebufferObject *fbo_;
    QOpenGLContext *context_;
    QSize size_;
};

}

#endif
