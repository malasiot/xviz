#ifndef XVIZ_QT_SCENE_VIEWER
#define XVIZ_QT_SCENE_VIEWER


#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/camera.hpp>

#include <Eigen/Geometry>

#include "renderer.hpp"
#include "trackball.hpp"

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QElapsedTimer>

class SceneViewer : public QOpenGLWidget
{
    Q_OBJECT

public:

    enum UpAxis { XAxis, YAxis, ZAxis } ;

    SceneViewer(const xviz::ScenePtr &scene, QWidget *parent = nullptr)  ;

    static void initDefaultGLContext() {

        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setMajorVersion(3);
        format.setMinorVersion(3);
        format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        format.setSwapInterval(1);

        format.setSamples(4);
        format.setProfile(QSurfaceFormat::CoreProfile);

        QSurfaceFormat::setDefaultFormat(format);
    }

    void setDrawAxes(bool draw_axes) {
        draw_axes_ = draw_axes ;
    }

    // should be called to initialized camera and trackball with given scene center and radius

    void initCamera(const Eigen::Vector3f &c, float r, UpAxis upAxis = YAxis );

    void setScene(const xviz::ScenePtr &scene) ;

    void startAnimations() ;

    void setAnimationCallback(std::function<void(float)> cb) {
        anim_cb_ = cb ;
    }

    virtual void onUpdate(float delta) {}

public slots:

    void updateAnimation() ;


protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void wheelEvent ( QWheelEvent * event ) override;

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    xviz::ScenePtr scene_ ;

    xviz::CameraPtr camera_ ;

    Renderer rdr_ ;
    TrackBall trackball_ ;

    bool draw_axes_ = true ;
    UpAxis axis_ = YAxis ;
    float aradius_ ;

    QElapsedTimer et_ ;
    std::function<void(float)> anim_cb_ = nullptr ;



};


#endif
