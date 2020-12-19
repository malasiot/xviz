#include <xviz/qt/scene/viewer.hpp>

#include <xviz/scene/scene.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/node.hpp>
#include <QTimer>

using namespace std ;
using namespace Eigen ;

SceneViewer::SceneViewer(const xviz::ScenePtr &scene, QWidget *parent): QOpenGLWidget(parent), scene_(scene) {
    setFocusPolicy(Qt::StrongFocus) ;

    //scene_.reset(new xviz::Scene) ;
   // scene_->load("/home/malasiot/Downloads/BoxTextured.gltf", xviz::Scene::IMPORT_LIGHTS);

    xviz::DirectionalLight *dl = new xviz::DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->diffuse_color_ = Vector3f(0.5, 0.5, 0.5) ;
    xviz::LightPtr light(dl) ;

    xviz::NodePtr node(new xviz::Node) ;
    node->setLight(light) ;
    scene_->addNode(node) ;
    scene_->addLight(light) ;

    auto c = scene_->geomCenter() ;
    auto r = scene_->geomRadius(c) ;
    initCamera(c, r, UpAxis::YAxis) ;

  //  scene_->addLight(xviz::LightPtr(dl)) ;
    //   cout << "ok" << endl ;
}

void SceneViewer::initCamera(const Vector3f &c, float r, UpAxis axis) {

   camera_.reset(new xviz::PerspectiveCamera(1.0, 70*M_PI/180, 0.01*r, 100*r)) ;

   axis_ = axis ;

   aradius_ = 10 * r ;

   if ( axis == YAxis )
       trackball_.setCamera(camera_, c + Vector3f{0.0, 0, 4*r}, c, {0, 1, 0}) ;
   else if ( axis == XAxis )
       trackball_.setCamera(camera_, c + Vector3f{0.0, 0, 4*r}, c, {1, 0, 0}) ;
   else if ( axis == ZAxis )
       trackball_.setCamera(camera_, c + Vector3f{0.0, 4*r, 0.0}, c, {0, 0, 1});

   trackball_.setZoomScale(0.1*r) ;

   camera_->setBgColor({1, 1, 1, 1}) ;
}

void SceneViewer::startAnimations()
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateAnimation()));

    et_.start() ;
    timer->start(30);
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
    }
    trackball_.setClickPoint(event->x(), event->y()) ;
    trackball_.update() ;

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

    trackball_.setScrollDirection(event->delta()>0);
    trackball_.update() ;
    update() ;
}


void SceneViewer::initializeGL() {
    if ( scene_ )
        rdr_.init(scene_) ;

}

void SceneViewer::resizeGL(int w, int h) {
    if ( camera_ ) {
        float ratio = w/(float)h ;
        std::static_pointer_cast<xviz::PerspectiveCamera>(camera_)->setAspectRatio(ratio) ;

        trackball_.setScreenSize(w, h);
        camera_->setViewport(w, h) ;
    }
}


void SceneViewer::paintGL()
{
    if ( !scene_ ) return ;

    rdr_.render(camera_) ;
#if 0
    if ( draw_axes_ ) {
        rdr_.clearZBuffer();

        rdr_.line({0, 0, 0}, {aradius_, 0, 0}, {1, 0, 0, 1}, 3);
        rdr_.line({0, 0, 0}, {0, aradius_, 0}, {0, 1, 0, 1}, 3);
        rdr_.line({0, 0, 0}, {0, 0, aradius_}, {0, 0, 1, 1}, 3);

        rdr_.text("X", Vector3f{aradius_, 0, 0}, Font("Arial", 12), Vector3f{1, 0, 0}) ;
        rdr_.text("Y", Vector3f{0, aradius_, 0}, Font("Arial", 12), Vector3f{0, 1, 0}) ;
        rdr_.text("Z", Vector3f{0, 0, aradius_}, Font("Arial", 12), Vector3f{0, 0, 1}) ;

        if ( axis_ == YAxis )
            rdr_.circle({0, 0, 0}, {0, 1, 0}, 5.0, {0, 1, 0, 1}) ;
        else if ( axis_ == XAxis )
            rdr_.circle({0, 0, 0}, {1, 0, 0}, 5.0, {0, 1, 0, 1}) ;
        else if ( axis_ == ZAxis )
            rdr_.circle({0, 0, 0}, {0, 0, 1}, 5.0, {0, 1, 0, 1}) ;
    }
#endif
}

void SceneViewer::updateAnimation() {

    float elapsed = et_.elapsed() ;

    if ( anim_cb_ != nullptr )
        anim_cb_(elapsed) ;
    else
        onUpdate(elapsed) ;

    et_.restart() ;

    update() ;

}
