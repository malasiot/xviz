#include <xviz/gui/viewer.hpp>

#include <xviz/scene/scene.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/node_helpers.hpp>
#include <random>

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

    PhongMaterial *material = new PhongMaterial(clr) ;

    MaterialPtr mat(material) ;
    box_node->addDrawable(geom, mat) ;

    box_node->setTransform(getRandTransform(0)) ;

    scene->addChild(box_node) ;

    return box_node ;
}

NodePtr randomCylinder(ScenePtr &scene, const string &name, float r, float h, const Vector4f &clr) {

    NodePtr box_node(new Node) ;
    box_node->setName(name) ;

    GeometryPtr geom(new CylinderGeometry(r, h)) ;

    PhongMaterial *material = new PhongMaterial(clr) ;

    MaterialPtr mat(material) ;
    box_node->addDrawable(geom, mat) ;

    box_node->setTransform(getRandTransform(0)) ;

    scene->addChild(box_node) ;

    return box_node ;
}

class Viewer: public SceneViewer {

public:
    Viewer(ScenePtr scene): SceneViewer(scene) {
        setDefaultCamera();
    }

    void onUpdate(float delta) override {
        return;
        NodePtr node = scene_->findNodeByName("ground") ;
        GeometryPtr geom = node->drawables()[0].geometry() ;
        auto &vertices = geom->vertices() ;

        for( int i=0 ; i<vertices.size() ; i++ ) {
            vertices[i].y() -= 0.01 ;
        }
        geom->setVerticesUpdated(true) ;

    }


};

int main(int argc, char **argv)
{
    ScenePtr scene(new Scene) ;

    scene->addChild(NodeHelpers::makeAxes(0.5)) ;

    NodePtr ground(new Node) ;
    ground->setName("ground");
    ground->transform().translate(Vector3f{0, -0.3, 0}) ;

    GeometryPtr plane(new Geometry(std::move(Geometry::makePlane(2, 2, 2, 2)))) ;

    MaterialPtr planeMat(new PhongMaterial({0.3, 0.3, 0.3}, 1)) ;
  //  planeMat->setSide(Material::Side::Both) ;
    ground->addDrawable(plane, planeMat) ;
    scene->addChild(ground) ;

    scene->addChild(NodeHelpers::makeCircle({0, 0, 0}, {1, 0, 0}, 2, {1, 0, 0})) ;

    GeometryPtr torus(new Geometry(std::move(Geometry::createSolidTorus(2, 0.2, 21, 61)))) ;
    PhongMaterial *material = new PhongMaterial({0, 0, 1}, 1) ;
    material->setShininess(20);
    material->setSpecularColor({1, 1, 1});

    MaterialPtr mat(material) ;
    NodePtr torus_node(new Node) ;
    torus_node->addDrawable(torus, mat) ;
    scene->addChild(torus_node) ;

    for( unsigned int i=0 ; i<10 ; i++ ) {
        Vector4f clr(0.5, rnd_uniform(0.0, 1.0), rnd_uniform(0.0, 1.0), 1.0) ;
        stringstream strm ;
        strm << "box" << i << endl ;

        if ( i %2 ) randomBox(scene, strm.str(), Vector3f(0.04, rnd_uniform(0.1, 0.15), 0.04), clr);
        else randomCylinder(scene, strm.str(), rnd_uniform(0.05, 0.1), rnd_uniform(0.1, 0.15), clr);
    }

    SpotLight *dl = new SpotLight(Vector3f(0, 1, 0), Vector3f(0, -1, 0)) ;
    dl->inner_cutoff_angle_ = 45 ; dl->outer_cutoff_angle_ =60;

    dl->linear_attenuation_ = 0.09 ;
    dl->quadratic_attenuation_ = 0.032 ;
    dl->diffuse_color_ = Vector3f(1.0, 0.5, 0.5) ;
    scene->addLightNode(LightPtr(dl)) ;

    xviz::DirectionalLight *dl2 = new xviz::DirectionalLight(Vector3f(0, 4, 4)) ;
    dl2->diffuse_color_ = Vector3f(0.15, 0.15, 0.15) ;
    dl2->shadow_cam_left_ = dl2->shadow_cam_top_ = -0.5 ;
    dl2->shadow_cam_right_ = dl2->shadow_cam_bottom_ = 0.5 ;
    dl2->shadow_cam_near_ = 0.01 ; dl2->shadow_cam_far_ = 3 ;
    dl2->shadow_bias_ = 0.0005;

    dl2->casts_shadows_ = true;
    xviz::LightPtr light2(dl2) ;
   scene->addLightNode(light2) ;

    QApplication app(argc, argv);

    SceneViewer::initDefaultGLContext();

    QMainWindow window ;
    window.setCentralWidget(new Viewer(scene)) ;
    window.resize(1024, 1024) ;
    window.show() ;

    return app.exec();
}
