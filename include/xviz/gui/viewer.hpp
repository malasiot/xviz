#ifndef XVIZ_QT_SCENE_VIEWER
#define XVIZ_QT_SCENE_VIEWER

#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/camera.hpp>

#include <Eigen/Geometry>

#include <xviz/scene/renderer.hpp>
#include <xviz/gui/trackball.hpp>

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QElapsedTimer>

namespace xviz {

class SceneViewer : public QOpenGLWidget
{
    Q_OBJECT

public:

    enum UpAxis { XAxis, YAxis, ZAxis } ;

    SceneViewer(const NodePtr &scene, QWidget *parent = nullptr)  ;

    static void initDefaultGLContext() {

        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setMajorVersion(3);
        format.setMinorVersion(3);
        format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        format.setSwapInterval(1);
        format.setOption(QSurfaceFormat::DebugContext);

        format.setSamples(4);
        format.setProfile(QSurfaceFormat::CoreProfile);

        QSurfaceFormat::setDefaultFormat(format);
    }

    void setDrawAxes(bool draw_axes);

    // should be called to initialized camera and trackball with given scene center and radius

    void initCamera(const Eigen::Vector3f &c, float r, UpAxis upAxis = YAxis );

    void setScene(const ScenePtr &scene) ;

    void startAnimations() ;

public slots:

    void updateAnimation() ;


protected:

    virtual void onUpdate(float delta) {}

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void wheelEvent ( QWheelEvent * event ) override;

    void keyPressEvent(QKeyEvent *event) override ;

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    void drawText(const Eigen::Vector3f &c, const QString &label, const QColor &clr) ;

    NodePtr scene_, axes_ ;
    CameraPtr camera_ ;

    Renderer rdr_ ;
    TrackBall trackball_ ;

    bool draw_axes_ = false ;
    UpAxis axis_ = YAxis ;
    float aradius_, radius_ ;
    float ts_ = 0 ;

    QElapsedTimer et_ ;

};

}


#endif
