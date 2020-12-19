#ifndef XVIZ_QT_OFFSCREEN_HPP
#define XVIZ_QT_OFFSCREEN_HPP

#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>
#include <QOpenGLContext>

#include <xviz/scene/scene_fwd.hpp>

class QOffscreenRenderer : public QOffscreenSurface
{
    Q_OBJECT

public:

    explicit QOffscreenRenderer(QScreen* targetScreen = nullptr, const QSize& size = QSize (1, 1));
    ~QOffscreenRenderer();

    QImage render(const xviz::ScenePtr &scene, const xviz::CameraPtr &cam);

private:
    QOpenGLFramebufferObject *fbo_;
    QOpenGLContext *context_;
    QSize size_;
};

#endif
