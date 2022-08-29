#include "bvh.hpp"

#include "mhx2_importer.hpp"
#include "mhx2_viz_scene.hpp"

#include <xviz/gui/viewer.hpp>

#include <cvx/misc/format.hpp>
#include <cvx/misc/strings.hpp>


#include <QMainWindow>
#include <QApplication>

#include "../util.hpp"

using namespace std ;
using namespace Eigen ;
using namespace cvx ;
using namespace xviz ;


int main(int argc, char *argv[]) {

    TestApplication app("skeleton", argc, argv) ;

    Mhx2Importer importer ;
    importer.load("/home/malasiot/Downloads/human-cmu.mhx2", "Human_cmu:Body") ;

    NodePtr model(new MHNode(importer.getModel())) ;

    BVHFile bvh ;
    bvh.parse("/home/malasiot/Downloads/cmu-mocap/data/001/01_02.bvh") ;
    bvh.drawJoints("/tmp/joints.obj", 100) ;

    BVHJoint *j = bvh.findJoint("LeftArm") ;

    Matrix3f t0 = j->lt_[0].block<3, 3>(0, 0) ;
    Matrix3f t1 = j->lt_[60].block<3, 3>(0, 0) ;

    Matrix3f r = t1 * t0.inverse() ;

    auto jn = model->findNodeByName("LeftArm") ;
    jn->transform().matrix().block<3, 3>(0, 0) = jn->transform().matrix().block<3, 3>(0, 0) * t1.inverse();

    SceneViewer::initDefaultGLContext();
    SceneViewer *viewer = new SceneViewer(model);
    viewer->setDefaultCamera() ;


    QMainWindow window ;
    window.setCentralWidget(viewer) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();




/*
    BVHFile bvh ;
    bvh.parse("/home/malasiot/Downloads/cmu/01/01_02.bvh") ;
    bvh.drawJoints("/tmp/joints.obj", 10) ;

    BVHJoint *j = bvh.findJoint("LeftArm") ;

    Matrix3f t0 = j->lt_[0].block<3, 3>(0, 0) ;
    Matrix3f t1 = j->lt_[10].block<3, 3>(0, 0) ;

    Matrix3f r = t1 * t0 ;
*/
 //   Pose p ;

/*
    p.setGlobalScale(1.0) ;
    p.setBoneTransform("LeftArm", Quaternionf(r)) ;
*/
 //   vector<Vector3f> mpos ;
 //   sk.getTransformedVertices(p, mpos) ;
  //  writeToOBJ("/tmp/mesh.obj", mpos) ;
}
