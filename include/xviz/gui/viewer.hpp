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

class Manipulator ;
using ManipulatorPtr = std::shared_ptr<Manipulator> ;

class SceneViewer : public QOpenGLWidget
{
    Q_OBJECT

public:

    enum UpAxis { XAxis, YAxis, ZAxis } ;

    SceneViewer(const NodePtr &scene, QWidget *parent = nullptr)  ;
    SceneViewer(QWidget *parent = nullptr)  ;
    ~SceneViewer() ;

    static void initDefaultGLContext();

    void setScene(const NodePtr &s) ;

    void setDefaultCamera() ;

    void makeAxes(float r) ;

    void setDrawAxes(bool draw_axes);

    // should be called to initialized camera and trackball with given scene center and radius

    void initCamera(const Eigen::Vector3f &c, float r, UpAxis upAxis = YAxis );

    void startAnimations() ;

    void addManipulator(const ManipulatorPtr &m) ;

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

    const std::vector<ManipulatorPtr> &getManipulators();

    NodePtr scene_, axes_ ;
    CameraPtr camera_ ;

    Renderer rdr_ ;
    TrackBall trackball_ ;

    bool draw_axes_ = false ;
    UpAxis axis_ = YAxis ;
    float aradius_, radius_ ;
    float ts_ = 0 ;

    QElapsedTimer et_ ;

    std::vector<ManipulatorPtr> manipulators_ ;

};

}


#endif
