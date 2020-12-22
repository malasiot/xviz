#include <xviz/qt/scene/viewer.hpp>

#include <xviz/scene/scene.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/light.hpp>

#include <random>
#include <iostream>

#include <QMainWindow>
#include <QApplication>

using namespace xviz ;
using namespace Eigen ;
using namespace std ;

std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 g_rng(rd()); //Standard mersenne_twister_engine seeded with rd()

static float rnd_uniform(float a, float b) {
   std::uniform_real_distribution<> dis(a, b);
   return dis(g_rng) ;
}


Matrix3f makeSkewSymmetric(const Vector3f& v) {
    Matrix3f result = Matrix3f::Zero();

    result(0, 1) = -v(2);
    result(1, 0) =  v(2);
    result(0, 2) =  v(1);
    result(2, 0) = -v(1);
    result(1, 2) = -v(0);
    result(2, 1) =  v(0);

    return result;
}

#define EPSILON_EXPMAP_THETA 1.0e-3

Eigen::Matrix3f expMapRot(const Vector3f& q) {
    float theta = q.norm();

    Matrix3f R = Matrix3f::Zero();
    Matrix3f qss =  makeSkewSymmetric(q);
    Matrix3f qss2 =  qss*qss;

    if (theta < EPSILON_EXPMAP_THETA)
        R = Matrix3f::Identity() + qss + 0.5*qss2;
    else
        R = Eigen::Matrix3f::Identity()
                + (sin(theta)/theta)*qss
                + ((1-cos(theta))/(theta*theta))*qss2;

    return R;
}

Isometry3f getRandTransform(double d)
{
    Isometry3f t = Isometry3f::Identity();

    Vector3f rotation(rnd_uniform(-M_PI, M_PI),  rnd_uniform(-M_PI, M_PI), rnd_uniform(-M_PI, M_PI)) ;
    Vector3f position(rnd_uniform(-0.8, 0.8), rnd_uniform(d, d + 0.1), rnd_uniform(-0.8, 0.8));

    t.translation() = position;
    t.linear() = expMapRot(rotation);

    return t;
}

NodePtr randomBox(ScenePtr &scene, const string &name, const Vector3f &hs, const Vector4f &clr) {

    NodePtr box_node(new Node) ;
    box_node->setName(name) ;

    GeometryPtr geom(new BoxGeometry(hs)) ;

    ConstantMaterial *material = new ConstantMaterial(clr) ;

    MaterialPtr mat(material) ;
    box_node->addDrawable(geom, mat) ;

    box_node->matrix() = getRandTransform(0) ;

    scene->addNode(box_node) ;

    return box_node ;
}

class PickingViewer: public SceneViewer {
public:
    PickingViewer(ScenePtr scene, QWidget *parent = nullptr): SceneViewer(scene, parent),
    ray_caster_(scene) {
        setMouseTracking(true);
   //    ray_caster_.buildOctrees();
        highlight_.reset(new ConstantMaterial({1, 0, 0, 1})) ;

    }

    void mouseMoveEvent(QMouseEvent *event) override {
        int x = event->x() ;
        int y = event->y() ;

        Ray ray = camera_->getRay(x, y) ;

        RayCastResult result ;
        if ( ray_caster_.intersect(ray, result) ) {
            cout << result.node_->name() << ' '
                 << result.triangle_idx_[0] << ' ' <<
                    result.triangle_idx_[1] << ' ' << result.triangle_idx_[2] << endl ;
            if ( result.drawable_ != selected_ ) {
                if ( old_ ) selected_->setMaterial(old_) ;
                selected_ = result.drawable_ ;
                old_ = selected_->material() ;
                selected_->setMaterial(highlight_) ;
                update() ;
            }
        } else {
            if ( selected_ ) {
                selected_->setMaterial(old_) ;
                selected_ = nullptr ;
                old_ = nullptr ;
                update() ;
            }
        }

        SceneViewer::mouseMoveEvent(event) ;
    }


private:
    RayCaster ray_caster_ ;
    xviz::MaterialPtr highlight_, old_;
    Drawable *selected_ = nullptr ;
};

int main(int argc, char **argv)
{
    ScenePtr model(new Scene) ;
    model->load("/home/malasiot/Downloads/2CylinderEngine.glb", 0);

    ScenePtr scene(new Scene) ;

    NodePtr modelNode(new Node) ;
    Affine3f tr(Affine3f::Identity());
    tr.scale(0.001f) ;
    tr.translate(-model->geomCenter()) ;
    modelNode->setTransform(tr) ;
    modelNode->addScene(model) ;
    modelNode->setName("model") ;
    scene->addNode(modelNode) ;

    for( uint i=0 ; i<10 ; i++ ) {
        Vector4f clr(0.5, rnd_uniform(0.0, 1.0), rnd_uniform(0.0, 1.0), 1.0) ;
        stringstream strm ;
        strm << "box" << i ;

        randomBox(scene, strm.str(), Vector3f(0.04, rnd_uniform(0.1, 0.15), 0.04), clr);
    }

    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->diffuse_color_ = Vector3f(1, 1, 1) ;
    scene->addLight(LightPtr(dl)) ;

    QApplication app(argc, argv);

    QSurfaceFormat format;
      format.setDepthBufferSize(24);
      format.setMajorVersion(3);
      format.setMinorVersion(3);

      format.setSamples(4);
      format.setProfile(QSurfaceFormat::CoreProfile);

      QSurfaceFormat::setDefaultFormat(format);

    QMainWindow window ;
    window.setCentralWidget(new PickingViewer(scene)) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
