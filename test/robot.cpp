#include <xviz/robot/robot_scene.hpp>
#include <xviz/gui/viewer.hpp>

#include <xviz/scene/light.hpp>

#include <QMainWindow>
#include <QApplication>

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


int main(int argc, char *argv[]) {


//    string package_path = "/home/malasiot/source/radioroso_ws/src/clopema_testbed/clopema_description/" ;
//    RobotScenePtr scene = RobotScene::loadURDF(package_path + "robots/clopema.urdf", { { "clopema_description", package_path } }, false) ;

  //  vector<string> hidden_nodes = { "ctu_f_roof_1", "ctu_f_roof_2", "ctu_f_roof_3", "ctu_f_wall_1",
    //                                "ctu_f_wall_2", "ctu_f_wall_3", "ctu_f_wall_4" };

//    for( const auto &s: hidden_nodes )
 //       scene->findNodeByName(s)->setVisible(false) ;

   string path = "/home/malasiot/local/bullet3/examples/pybullet/gym/pybullet_data/pr2_gripper.urdf" ;
  //  string path ="/home/malasiot/local/bullet3/examples/pybullet/gym/pybullet_data/cartpole.urdf";
    auto robot = URDFRobot::load(path) ;
    RobotScenePtr scene = RobotScene::fromURDF(robot) ;

    DirectionalLight *dl = new DirectionalLight(Vector3f(1.5, 2.5, 1)) ;
    dl->diffuse_color_ = Vector3f(1, 1, 1) ;
    scene->addLightNode(LightPtr(dl)) ;

    QApplication app(argc, argv);
    SceneViewer::initDefaultGLContext();

    QMainWindow window ;
    window.setCentralWidget(new RobotViewer(robot, scene)) ;
    window.resize(1024, 1024) ;
    window.show() ;

    return app.exec();

}
