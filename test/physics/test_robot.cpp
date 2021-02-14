#include <xviz/scene/camera.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/node.hpp>

#include <xviz/scene/material.hpp>
#include <xviz/scene/geometry.hpp>

#include <xviz/robot/robot_scene.hpp>
#include <xviz/scene/node_helpers.hpp>
#include <xviz/physics/world.hpp>
#include <xviz/physics/multi_body.hpp>

#include "bullet_gui.hpp"

#include <QApplication>
#include <QMainWindow>


#include <iostream>
#include <thread>

using namespace xviz ;


using namespace std ;
using namespace Eigen ;

PhysicsWorld physics ;
ScenePtr scene(new Scene) ;

MultiBodyPtr body(new MultiBody) ;

class GUI: public SimulationGui, CollisionFeedback {
public:
    GUI(xviz::ScenePtr scene, xviz::PhysicsWorld &physics):
    SimulationGui(scene, physics) {
        physics_.setCollisionFeedback(this) ;
        collision_sensor_.reset(new GhostObject(CollisionShapePtr(new SphereCollisionShape(0.1)))) ;
        Isometry3f tr = Isometry3f::Identity() ;
        tr.translate(Vector3f(0, 0, -0.5)) ;
        collision_sensor_->setWorldTransform(tr) ;
        physics_.addGhost(collision_sensor_) ;
    }

    void onUpdate(float delta) override {
        map<string, Isometry3f> transforms ;
        body->getLinkTransforms(transforms) ;
        scene->updateTransforms(transforms) ;
        SimulationGui::onUpdate(delta) ;
        cout << collision_sensor_->isOverlapping() << endl ;



    }

    void processContact(ContactResult &r) override {
        if ( r.a_ == nullptr || r.b_ == nullptr ) return ;
        if ( r.a_->getName() == "ground" || r.b_->getName() == "ground" ) return  ;
        cout << r.a_->getName() << ' ' << r.b_->getName() << endl ;
    }

    GhostObjectPtr  collision_sensor_ ;


};

NodePtr makeCube(const string &name, const Vector3f &hs, const Vector4f &clr, NodePtr parent) {
    NodePtr node = NodeHelpers::makeBox(hs, clr) ;
    node->setName(name) ;
    parent->addChild(node) ;
    return node ;
}

void createScene() {

    physics.createMultiBodyDynamicsWorld();

    Affine3f tr(Translation3f{0, -1.6, 0}) ;

    Vector3f ground_hs{3.5f, 0.05f, 3.5f} ;
    auto groundNode = NodeHelpers::makeBox(ground_hs, {0.5, 0.5, 0.5, 1}) ;
    groundNode->setTransform(tr) ;
    scene->addChild(groundNode) ;
    RigidBodyPtr ground_rb(new RigidBody(CollisionShapePtr(new BoxCollisionShape(ground_hs)), tr));
    ground_rb->setName("ground");
    physics.addRigidBody(ground_rb) ;

    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->diffuse_color_ = Vector3f(1, 1, 1) ;
    scene->addLightNode(LightPtr(dl)) ;

    float link_size = 0.4 ;
    Vector3f box_hs{0.05, link_size/2, 0.05} ;
    float box_mass = 0.1 ;

    CollisionShapePtr box_shape(new BoxCollisionShape(box_hs)) ;

    Isometry3f offset ;
    offset.setIdentity() ;
    offset.translate(Vector3f{0, -link_size/2, 0}) ;

    body->addLink("base", 0.0, box_shape, offset).setLocalInertialFrame(offset) ;
    body->addLink("link1", box_mass, box_shape, offset).setLocalInertialFrame(offset) ;
    body->addLink("link2", box_mass, box_shape, offset).setLocalInertialFrame(offset) ;
    body->addLink("link3", box_mass, box_shape, offset).setLocalInertialFrame(offset) ;

    Vector3f axis = {1, 0, 0} ;
    Isometry3f j2p ;
    j2p.setIdentity() ;
    j2p.translate(Vector3f{0, -link_size, 0}) ;
    auto &j1 = body->addJoint("j1", RevoluteJoint, "base", "link1", j2p).setAxis(axis) ;
    auto &j2 = body->addJoint("j2", RevoluteJoint, "link1", "link2", j2p).setAxis(axis) ;
    auto &j3 = body->addJoint("j3", RevoluteJoint, "link2", "link3", j2p).setAxis(axis) ;

    physics.addMultiBody(body) ;

    j1.setMotorMaxImpulse(0) ;
    j2.setMotorMaxImpulse(0) ;
    j3.setMotorMaxImpulse(0) ;

    NodePtr base_node = makeCube("base", box_hs, {1, 0, 0, 1}, scene);
    NodePtr link1_node = makeCube("link1", box_hs, {1, 1, 0, 1}, scene) ;
    NodePtr link2_node = makeCube("link2", box_hs, {1, 0, 1, 1}, scene) ;
    NodePtr link3_node = makeCube("link3", box_hs, {0, 0, 1, 1}, scene) ;

    Vector3f col_hs{0.1f, 0.1f, 0.1f} ;
    Isometry3f col_tr = Isometry3f::Identity();
    col_tr.translate(Vector3f{0.0, -1.5+0.2, -0.25});
    NodePtr cube = makeCube("cube", col_hs, {1, 0 , 0, 1}, scene) ;
    cube->setTransform(col_tr) ;
    RigidBodyPtr cube_rb(new RigidBody(0.15, new UpdateSceneMotionState(cube), CollisionShapePtr(new BoxCollisionShape(col_hs)))) ;
    cube_rb->setName("cube") ;
    physics.addRigidBody(cube_rb) ;
}





int main(int argc, char **argv)
{
    createScene() ;

    QApplication app(argc, argv);

    SceneViewer::initDefaultGLContext() ;

    QMainWindow window ;
    window.setCentralWidget(new GUI(scene, physics)) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
