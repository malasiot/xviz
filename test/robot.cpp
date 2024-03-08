#include <xviz/robot/robot_scene.hpp>
#include <xviz/gui/viewer.hpp>

#include <xviz/scene/light.hpp>
#include <xviz/scene/geometry.hpp>

#include <QMainWindow>
#include <QApplication>
#include <QDebug>

#include "util.hpp"

using namespace std ;
using namespace xviz ;
using namespace Eigen ;


class RobotViewer: public SceneViewer {
public:
    RobotViewer(URDFRobot &rb, RobotScenePtr scene, QWidget *parent = nullptr): SceneViewer(scene, parent), urdf_(rb) {
        setDefaultCamera() ;
        camera_->setBgColor({0.4f, 0.4f, 0.4f, 1.0f});
    }

    void	keyPressEvent(QKeyEvent *event) override {
        int key = event->key() ;

        if ( key == Qt::Key_Q ) {
            v = urdf_.setJointPosition("left_gripper_joint", v - 0.1) ;
              qDebug() << v ;
        } else if ( key == Qt::Key_W ) {
            v = urdf_.setJointPosition("left_gripper_joint", v + 0.1) ;
        }
        map<string, Isometry3f> transforms ;
        urdf_.computeLinkTransforms(transforms);

        scene_->updateTransforms(transforms) ;

        update() ;
    }

    URDFRobot &urdf_ ;
    float v = 0 ;
};

NodePtr makeBox(const string &name, const Vector3f &hs, const Vector3f &tr, const Vector4f &clr) {

    NodePtr box_node(new Node) ;
    box_node->setName(name) ;

    GeometryPtr geom(new BoxGeometry(hs)) ;

    ConstantMaterial *material = new ConstantMaterial(clr) ;

    MaterialPtr mat(material) ;
    box_node->addDrawable(geom, mat) ;

    box_node->setTransform(Isometry3f(Translation3f(tr))) ;


    return box_node ;
}

int main(int argc, char *argv[]) {
    TestApplication app("robot", argc, argv) ;
//    string package_path = "/home/malasiot/source/radioroso_ws/src/clopema_testbed/clopema_description/" ;
//    RobotScenePtr scene = RobotScene::loadURDF(package_path + "robots/clopema.urdf", { { "clopema_description", package_path } }, false) ;

  //  vector<string> hidden_nodes = { "ctu_f_roof_1", "ctu_f_roof_2", "ctu_f_roof_3", "ctu_f_wall_1",
    //                                "ctu_f_wall_2", "ctu_f_wall_3", "ctu_f_wall_4" };

//    for( const auto &s: hidden_nodes )
 //       scene->findNodeByName(s)->setVisible(false) ;

  // string path = TestApplication::data() + "robots/pr2_gripper.urdf" ;
   string path ="/home/malasiot/source/xsim/data/robots/ur5/ur5_robotiq85_gripper.urdf";
    auto robot = URDFRobot::loadFile(path) ;
    robot.setJointPosition("elbow_joint", 0.5) ;
       robot.setJointPosition("shoulder_lift_joint", -.54) ;
    RobotScenePtr scene = RobotScene::fromURDF(robot) ;

    map<string, Isometry3f> transforms ;
    robot.computeLinkTransforms(transforms);

    scene->updateTransforms(transforms) ;

    scene->addChild(makeBox("box1", {0.005, 0.005, 0.05}, {0.25, 0.15, 0.355}, {1, 0, 0, 1}));
    scene->addChild(makeBox("box2", {0.05, 0.05, 0.05}, {0, 0.4, 0.1}, {1, 1, 0, 1}));

    DirectionalLight *dl = new DirectionalLight(Vector3f(1.5, 2.5, 1)) ;
    dl->setDiffuseColor(Vector3f(1, 1, 1)) ;
    scene->addLightNode(LightPtr(dl)) ;

    SceneViewer::initDefaultGLContext();

    QMainWindow window ;
    window.setCentralWidget(new RobotViewer(robot, scene)) ;
    window.resize(1024, 1024) ;
    window.show() ;

    return app.exec();

}
