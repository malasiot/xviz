#include <xviz/gui/viewer.hpp>
#include <xviz/gui/manipulator.hpp>
#include <xviz/scene/scene.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/node.hpp>
#include <xviz/scene/node_helpers.hpp>


#include <QTimer>
#include <QPainter>
#include <QApplication>

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

}

void SceneViewer::setScene(const NodePtr &scene) {
    scene_->children().clear() ;

    scene_->addChild(scene) ;

    bool has_light = false ;
    scene->visit([&has_light](const Node &node) {
       if ( node.light() ) has_light = true ;
    });

    if ( !has_light ) {
        auto dl = make_shared<DirectionalLight>(Vector3f(0.5, 0.5, 1))  ;
        dl->diffuse_color_ = Vector3f(0.75, 0.75, 0.75) ;
        scene_->addLightNode(dl) ;
    }

    auto c = scene_->geomCenter() ;
    auto r = scene_->geomRadius(c) ;
    initCamera(c, r, UpAxis::YAxis) ;

    axes_ = NodeHelpers::makeAxes(r) ;
    axes_->setVisible(false) ;
    scene_->addChild(axes_) ;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateAnimation()));

    et_.start() ;
    timer->start(30);
}

SceneViewer::~SceneViewer()
{

}

void SceneViewer::setDrawAxes(bool draw_axes) {
    draw_axes_ = draw_axes ;
    axes_->setVisible(draw_axes) ;
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

void SceneViewer::addManipulator(const ManipulatorPtr &m) {

    manipulators_.emplace_back(m) ;
    m->setCamera(camera_) ;
}

void SceneViewer::mousePressEvent(QMouseEvent *event)
{
    if ( !camera_ ) return ;

    for( const ManipulatorPtr &m: getManipulators() ) {
        if ( !m->isVisible() ) continue ;
        if ( m->onMousePressed(event) ) {
            return ;
        }
    }

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
    }
    trackball_.setClickPoint(event->x(), event->y()) ;
    trackball_.update() ;
}

void SceneViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if ( !camera_ ) return ;

    for( const ManipulatorPtr &m: getManipulators() ) {
        if ( !m->isVisible() ) continue ;
        if ( m->onMouseReleased(event) ) {
            update() ;
            return ;
        }
    }

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
    }
    trackball_.setClickPoint(event->x(), event->y()) ;
    trackball_.update() ;

}

void SceneViewer::mouseMoveEvent(QMouseEvent *event)
{
    if ( !camera_ ) return ;

    for( const ManipulatorPtr &m: getManipulators() ) {
        if ( !m->isVisible() ) continue ;
        if ( m->onMouseMoved(event) ) {
            return ;
        }
    }

    int x = event->x() ;
    int y = event->y() ;

    trackball_.setClickPoint(x, y) ;
    trackball_.update() ;

}

void SceneViewer::wheelEvent(QWheelEvent *event) {
    if ( !camera_ ) return ;

    trackball_.setScrollDirection(event->angleDelta().y()>0);
    trackball_.update() ;
    update() ;

    for( const ManipulatorPtr &m: getManipulators() ) {
        m->onCameraUpdated() ;
    }
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
    }

}


void SceneViewer::initializeGL() {
   rdr_.init(scene_) ;
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

const std::vector<ManipulatorPtr> &SceneViewer::getManipulators() {
    return manipulators_ ;
}

void SceneViewer::paintGL()
{
    if ( !scene_ ) return ;
    rdr_.setDefaultFBO(defaultFramebufferObject());
    rdr_.render(camera_) ;

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

}
