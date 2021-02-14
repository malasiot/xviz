#include <xviz/scene/camera.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/node.hpp>

#include <xviz/scene/material.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/node_helpers.hpp>
#include <xviz/robot/robot_scene.hpp>

#include <iostream>
#include <thread>

#include <xviz/physics/world.hpp>

#include "bullet_gui.hpp"

#include <xviz/physics/multi_body.hpp>

#include <QApplication>
#include <QMainWindow>

using namespace xviz ;

using namespace std ;
using namespace Eigen ;

PhysicsWorld physics ;
ScenePtr scene(new Scene) ;

MultiBodyPtr body(new MultiBody) ;

class GUI: public SimulationGui {
public:
    GUI(xviz::ScenePtr scene, xviz::PhysicsWorld &physics,
        URDFRobot &rb, const string &ctrl_joint):
        SimulationGui(scene, physics), robot_(rb), ctrl_joint_(ctrl_joint) {

        initCamera({0, -1, 0}, 0.5, SceneViewer::YAxis) ;
        vert_pos_ = 0.0, gripper_pos_ = 0.5 ;
        setDrawAxes(true);

    }

    void onUpdate(float delta) override {
        Joint *j= body->findJoint(ctrl_joint_) ;

        //       j->setMimicJointPosition();
        //       cout << body->getJointPosition(ctrl_joint_) << endl ;


        map<string, Isometry3f> transforms ;
        body->getLinkTransforms(transforms) ;
        scene->updateTransforms(transforms) ;
     SimulationGui::onUpdate(delta) ;

    }

    void keyPressEvent(QKeyEvent *event) override {

        if ( event->key() == Qt::Key_Q ) {
            vert_pos_ -= 0.05 ;
            body->setJointPosition("world_to_base", vert_pos_) ;
        } else if ( event->key() == Qt::Key_W ) {
            vert_pos_ += 0.05 ;
            body->setJointPosition("world_to_base", vert_pos_) ;
        } else if ( event->key() == Qt::Key_A ) {
            gripper_pos_ -= 0.03 ;
            body->setJointPosition("left_gripper_joint", gripper_pos_) ;
            body->setJointPosition("right_gripper_joint", gripper_pos_) ;
        }  else if ( event->key() == Qt::Key_S ) {
            gripper_pos_ += 0.03 ;
            body->setJointPosition("left_gripper_joint", gripper_pos_) ;
            body->setJointPosition("right_gripper_joint", gripper_pos_) ;
        }

        update() ;

    }

private:
    URDFRobot &robot_ ;
    float vert_pos_ = 0.0, gripper_pos_ = 0.5 ;
    string ctrl_joint_ ;
};

URDFRobot robot ;

void createScene() {

    physics.createMultiBodyDynamicsWorld();
    physics.setGravity({0, -10, 0});

    Affine3f tr(Translation3f{0, -1.5, 0}) ;

    Vector3f ground_hs{1.5f, 0.05f, 1.5f} ;
    NodePtr groundNode = NodeHelpers::makeBox(ground_hs, {0.5, 0.5, 0.5, 1}) ;
    groundNode->setTransform(tr) ;
    scene->addChild(groundNode) ;
    physics.addRigidBody(make_shared<RigidBody>(CollisionShapePtr(new BoxCollisionShape(ground_hs)), tr)) ;

    Affine3f box_tr(Translation3f{0, -1.25, 0}) ;
    Vector3f box_hs{0.03, 0.03f, 0.03f} ;
    NodePtr boxNode = NodeHelpers::makeBox(box_hs, {0.5, 0.5, 0, 1} );
    boxNode->setTransform(box_tr);
    scene->addChild(boxNode) ;
    physics.addRigidBody(make_shared<RigidBody>(2.0, new UpdateSceneMotionState(boxNode), CollisionShapePtr(new BoxCollisionShape(box_hs)))) ;


    string path = "/home/malasiot/local/bullet3/examples/pybullet/gym/pybullet_data/pr2_gripper.urdf" ;
    robot = URDFRobot::load(path, { }, true) ;

    RobotScenePtr rs = RobotScene::fromURDF(robot) ;

    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->diffuse_color_ = Vector3f(0.5, 0.5, 0.5) ;
    scene->addLightNode(LightPtr(dl)) ;

    scene->addChild(rs) ;

    Isometry3f global = Isometry3f::Identity() ;

    global.rotate(AngleAxisf(-M_PI/2, Vector3f::UnitZ())) ;
//    global.translate(Vector3f{0, 0, -0.2});


    body->loadURDF(robot) ;

   // body->addLink("world", 0.0, nullptr) ;
//    body->addJoint("world_to_base", PrismaticJoint, "world", "gripper_pole", global).setAxis(Vector3f::UnitX()) ;

    physics.addMultiBody(body) ;




}

int main(int argc, char **argv)
{
    createScene() ;

    QApplication app(argc, argv);

    SceneViewer::initDefaultGLContext() ;

    QMainWindow window ;
    window.setCentralWidget(new GUI(scene, physics, robot, "left_gripper_joint")) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
