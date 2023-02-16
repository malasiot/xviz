#include <xviz/gui/viewer.hpp>
#include <xviz/gui/manipulator.hpp>
#include <xviz/scene/scene.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/node.hpp>
#include <xviz/scene/node_helpers.hpp>


#include <QTimer>
#include <QPainter>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

using namespace std ;
using namespace Eigen ;

namespace xviz {
SceneViewer::SceneViewer(const NodePtr &scene, QWidget *parent): SceneViewer(parent) {
    setScene(scene) ;
}

SceneViewer::SceneViewer(QWidget *parent): QOpenGLWidget(parent), scene_(new Node)
{
    setFocusPolicy(Qt::StrongFocus) ;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true) ;

    grab_timer_ = new QTimer(this);
    connect(grab_timer_, &QTimer::timeout, this, &SceneViewer::grabScreen);
    grab_count_ = 0;

    QString path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0];
    configureFrameGrabber(path + "/grab", 30);
}

void SceneViewer::setScene(const NodePtr &scene) {
//    scene_->children().clear() ;

    scene_->addChild(scene) ;

    bool has_light = false ;
    scene->visit([&has_light](const Node &node) {
       if ( node.light() ) has_light = true ;
    });

    if ( !has_light ) {
        auto dl = make_shared<DirectionalLight>(Vector3f(0.5, 0.5, 1))  ;
        dl->setDiffuseColor({1, 1, 1}) ;
        scene_->addLightNode(dl) ;
    }
}

void SceneViewer::makeAxes(float r) {
     axes_ = NodeHelpers::makeAxes(r) ;
     axes_->setVisible(false) ;
     scene_->addChild(axes_) ;
}

void SceneViewer::setDefaultCamera() {
    assert(scene_) ;
    auto c = scene_->geomCenter() ;
    auto r = scene_->geomRadius(c) ;
    initCamera(c, r, UpAxis::YAxis) ;
}

SceneViewer::~SceneViewer()
{

}

void SceneViewer::initDefaultGLContext() {

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

void SceneViewer::setDrawAxes(bool draw_axes) {
    draw_axes_ = draw_axes ;
    if ( axes_ )
        axes_->setVisible(draw_axes) ;
    update() ;
}

void SceneViewer::initCamera(const Vector3f &c, float r, UpAxis axis) {
    camera_.reset(new PerspectiveCamera(1.0, 70*M_PI/180, 0.01*r, 100*r)) ;

    axis_ = axis ;

    radius_ = r ;
    aradius_ = 10 * r ;

    if ( axis == YAxis )
        trackball_.setCamera(camera_, c + Vector3f{0.0, 0, 4*r}, c, {0, 1, 0}) ;
   else if ( axis == XAxis )
        trackball_.setCamera(camera_, c + Vector3f{0.0, 0, 4*r}, c, {1, 0, 0}) ;
   else if ( axis == ZAxis )
       trackball_.setCamera(camera_, c + Vector3f{0.0, 4*r, 0.0}, c, {0, 0, 1});

   trackball_.setZoomScale(0.1*r) ;

   camera_->setBgColor({1, 1, 1, 1}) ;

   camera_->setViewport(width(), height()) ;

}

void SceneViewer::startAnimations()
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateAnimation()));

    et_.start() ;
    timer->start(30);
}

void SceneViewer::configureFrameGrabber(const QString &path, float fps) {
    grab_frame_path_ = path ;
    grab_timer_->setInterval(1000/fps) ;
}


void SceneViewer::mousePressEvent(QMouseEvent *event)
{
    if ( !camera_ ) return ;

    switch ( event->button() ) {
    case Qt::LeftButton:
        trackball_.setLeftClicked(true) ;
        break ;
    case Qt::MiddleButton:
        trackball_.setMiddleClicked(true) ;
        break ;
    case Qt::RightButton:
        trackball_.setRightClicked(true) ;
        break ;
    default: break ;
    }
    trackball_.setClickPoint(event->x(), event->y()) ;
    trackball_.update() ;
}

void SceneViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if ( !camera_ ) return ;

    switch ( event->button() ) {
    case Qt::LeftButton:
        trackball_.setLeftClicked(false) ;
        break ;
    case Qt::MiddleButton:
        trackball_.setMiddleClicked(false) ;
        break ;
    case Qt::RightButton:
        trackball_.setRightClicked(false) ;
        break ;
    default:
        break ;
    }
    trackball_.setClickPoint(event->x(), event->y()) ;
    trackball_.update() ;
    update() ;

}

void SceneViewer::mouseMoveEvent(QMouseEvent *event)
{
    if ( !camera_ ) return ;

    int x = event->x() ;
    int y = event->y() ;

    trackball_.setClickPoint(x, y) ;
    trackball_.update() ;
    update() ;

}

void SceneViewer::wheelEvent(QWheelEvent *event) {
    if ( !camera_ ) return ;

    trackball_.setScrollDirection(event->angleDelta().y()>0);
    trackball_.update() ;
    update() ;
}

void SceneViewer::keyPressEvent(QKeyEvent *event)
{
    int key = event->key() ;
    if ( key == Qt::Key_A ) {
        draw_axes_ = !draw_axes_ ;
        setDrawAxes(draw_axes_);
        update() ;
    } else if ( key == Qt::Key_Escape ) {
        QApplication::quit();
    } else if ( key == Qt::Key_R ) {
        startRecording();
    } else if ( key == Qt::Key_S ) {
        stopRecording() ;
    }

}


void SceneViewer::initializeGL() {
   rdr_.init() ;
}

void SceneViewer::resizeGL(int w, int h) {
    if ( camera_ ) {
        float ratio = w/(float)h  ;
        std::static_pointer_cast<PerspectiveCamera>(camera_)->setAspectRatio(ratio) ;

        trackball_.setScreenSize(w, h);
        camera_->setViewport(w, h) ;

    }
}

void SceneViewer::drawText(const Vector3f &c, const QString &text, const QColor &clr) {
    Vector2f p = rdr_.project(c) ;
    // Render text
    QPainter painter(this);

    QFontMetrics fm(painter.font()) ;
    QRect rect = fm.boundingRect(text) ;
    painter.setPen(clr) ;
    painter.drawText(p.x() - rect.width()/2, p.y() + rect.height()/2, text);
    painter.end();
}


void SceneViewer::paintGL()
{
    if ( !scene_ ) return ;
    rdr_.setDefaultFBO(defaultFramebufferObject());
    rdr_.render(scene_, camera_) ;

    if ( draw_axes_ ) {
        drawText(Vector3f{radius_, 0, 0}, "X", Qt::red) ;
        drawText(Vector3f{0, radius_, 0}, "Y", Qt::green) ;
        drawText(Vector3f{0, 0, radius_}, "Z", Qt::blue) ;
    }

}

void SceneViewer::updateAnimation() {

    float elapsed = et_.elapsed() ;

    scene_->updateAnimations(elapsed) ;

    float delta = elapsed - ts_ ;

    onUpdate(delta) ;

    ts_ = elapsed ;

//    et_.restart() ;

    update() ;

}

void SceneViewer::startRecording() {
    stopRecording() ;
    grab_count_ = 0 ;
    grab_timer_->start() ;
    emit recordingStarted();
}


void SceneViewer::grabScreen() {
    auto image = grabFramebuffer();

    if (grab_frame_path_.isEmpty()) {
        grab_frame_path_ = QDir::currentPath();
        grab_frame_path_ += "/grab" ;
    }

    QString file_name = QString("%1_%2.png").arg(grab_frame_path_).arg((int)grab_count_, 4, 10, QLatin1Char('0'));


    if (!image.save(file_name)) {
        qDebug() << "The image could not be saved to " << QDir::toNativeSeparators(file_name);
    }

    grab_count_++;
}

void SceneViewer::stopRecording() {
    grab_timer_->stop();
    grab_count_ = 0;
}

}
