#include <xviz/scene/material.hpp>

#include <QApplication>
#include <QMainWindow>

#include <xviz/scene/camera.hpp>
#include <xviz/scene/light.hpp>

#include <xviz/scene/node.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/mesh.hpp>

#include <iostream>

#include <xviz/physics/world.hpp>
#include <xviz/physics/soft_body.hpp>

#include "bullet_gui.hpp"

#include <xviz/scene/scene.hpp>
#include <xviz/scene/node_helpers.hpp>

using namespace Eigen ;

using namespace std ;
using namespace xviz ;

static PhysicsWorld physics ;
static ScenePtr scene ;

class TestClothWidget: public SimulationGui {
public:
    TestClothWidget(ScenePtr scene, PhysicsWorld &physics, const Vector3f &c, float r): SimulationGui(scene, physics, c, r) {
        node_ = scene_->findNodeByName("cloth") ;
        cloth_ = physics.findSoftBody("cloth");
    }

    void onUpdate(float delta) override {
        SimulationGui::onUpdate(delta) ;
        updateClothGeometry() ;
    }

    void updateClothGeometry() {
        auto &dr = node_->drawables()[0] ;

        GeometryPtr geom = dr.geometry() ;
        geom->vertices().clear() ;
        geom->normals().clear() ;
        cloth_->getVisualGeometry(geom->vertices(), geom->normals(), geom->indices());
        geom->setVerticesUpdated(true);
        geom->setNormalsUpdated(true);

    }

    SoftBodyPtr cloth_ ;
    NodePtr node_ ;
};



NodePtr makeBox(const string &name, const Vector3f &hs, const Matrix4f &tr, const Vector4f &clr) {

    NodePtr box_node(new Node) ;
    box_node->setName(name) ;

    GeometryPtr geom(new BoxGeometry(hs)) ;

    MaterialPtr material(new ConstantMaterial(clr)) ;

    box_node->addDrawable(geom, material) ;

    box_node->setTransform(Affine3f(tr)) ;

    return box_node ;
}

NodePtr makeCylinder(const string &name, float radius, float length, const Matrix4f &tr, const Vector4f &clr) {

    // we need an extra node to perform rotation of cylinder so that it is aligned with Y axis instead of Z

    NodePtr node(new Node) ;
    node->setName(name) ;

    GeometryPtr geom(new CylinderGeometry(radius, length)) ;

    MaterialPtr material(new ConstantMaterial(clr)) ;

    node->addDrawable(geom, material) ;

    node->transform().rotate(AngleAxisf(-0.5*M_PI, Vector3f::UnitX()));

    NodePtr externalNode(new Node) ;
    externalNode->setTransform(Affine3f(tr)) ;
    externalNode->addChild(node) ;

    return externalNode ;
}


// create cylinder aligned with Y axis
NodePtr makeCylinder(float radius, float length, const Eigen::Affine3f &tr, const Eigen::Vector4f &clr) {

    // we need an extra node to perform rotation of cylinder so that it is aligned with Y axis instead of Z

    NodePtr node(new Node) ;

    GeometryPtr geom(new CylinderGeometry(radius, length)) ;

    node->addDrawable(geom, MaterialPtr(new PhongMaterial(clr))) ;

    node->transform().rotate(Eigen::AngleAxisf(-0.5*M_PI, Eigen::Vector3f::UnitX()));

    NodePtr externalNode(new Node) ;
    externalNode->setTransform(tr) ;
    externalNode->addChild(node) ;

    return externalNode ;
}
void createScene() {
    scene.reset(new Scene) ;

    // init physics

    physics.createSoftBodyDynamicsWorld();

    // add light
    std::shared_ptr<DirectionalLight> dl( new DirectionalLight(Vector3f(0, 1, 0.1)) ) ;
    dl->diffuse_color_ = Vector3f(1, 1, 1) ;
    scene->addLightNode(dl) ;

    // create ground plane object

    Affine3f tr(Translation3f{0, -0.05, 0}) ;

    Vector3f ground_hs{10., 0.05, 10.} ;
    NodePtr groundNode = NodeHelpers::makeBox(ground_hs, { 0.5, 0.5, 0.5, 1}) ;
    groundNode->setTransform(tr) ;
    groundNode->setName("ground") ;
    scene->addChild(groundNode) ;

    RigidBodyPtr ground = make_shared<RigidBody>(CollisionShapePtr(new BoxCollisionShape(ground_hs)), tr);
    ground->setName("ground") ;
    physics.addRigidBody(ground) ;

    // create static pole
    Affine3f poleTransform ;
    poleTransform.setIdentity() ;
    poleTransform.translate(Vector3f{.15, 2.5, 0.5}) ;
    poleTransform.rotate(AngleAxisf(0.5*M_PI, Vector3f::UnitZ()));
      poleTransform.translate(Vector3f{-0.15, -2.5, 0.5}) ;
    auto poleNode = makeCylinder(0.25, 10, poleTransform, { 0, 1, 0, 1}) ;
    poleNode->setName("pole") ;

    scene->addChild(poleNode) ;
    RigidBodyPtr pole = make_shared<RigidBody>(CollisionShapePtr(new CylinderCollisionShape(0.25, 10)), poleTransform);
    pole->setName("pole") ;
    physics.addRigidBody(pole) ;


    const btScalar s = 4;  //size of cloth patch
    const int NUM_X = 31;  //vertices on X axis
    const int NUM_Z = 31;  //vertices on Z axis

    SoftBodyPtr sb(new SoftPatch2D(physics, {-2, 3, 0}, {2, 3, 0}, {-2, 3, 4}, NUM_X, NUM_Z, 1+2, false));
    sb->setName("cloth");
    physics.addSoftBody(sb) ;


    NodePtr node(new Node) ;

    node->setName("cloth") ;

    MaterialPtr material(new PhongMaterial({0, 1, 1, 1}));
    material->setSide(Material::Side::Both) ;

    GeometryPtr mesh(new Geometry(Geometry::Triangles));
    sb->getVisualGeometry(mesh->vertices(), mesh->normals(), mesh->indices()) ;
    node->addDrawable(mesh, material) ;

    scene->addChild(node) ;



}


int main(int argc, char **argv)
{

    createScene() ;

    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setSwapInterval(1);

    format.setSamples(4);
    format.setProfile(QSurfaceFormat::CoreProfile);

    QSurfaceFormat::setDefaultFormat(format);

    QMainWindow window ;
    window.setCentralWidget(new TestClothWidget(scene, physics, { 0, 0, 0}, 3.0)) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
