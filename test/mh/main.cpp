#include "bvh.hpp"

#include "mhx2_importer.hpp"
#include "mhx2_viz_scene.hpp"

#include <xviz/gui/viewer.hpp>

#include <cvx/misc/format.hpp>
#include <cvx/misc/strings.hpp>


#include <QMainWindow>
#include <QApplication>

#include "../util.hpp"


#include <iostream>

using namespace std ;
using namespace Eigen ;
using namespace cvx ;
using namespace xviz ;

using Pose = std::map<std::string, Eigen::Isometry3f> ;

bool load_pose_from_mhp(const string &fname, Pose &pose) {
    ifstream strm(fname.c_str()) ;

    while (strm.good())
    {
        string line ;
        getline(strm, line) ;

        if ( line.empty()  ) break ;

        stringstream ss(line) ;

        string bname ;
        ss >> bname ;

        Matrix4f a ;

        a.setIdentity();

        ss >> a(0, 0) >> a(0, 1) >> a(0, 2) >> a(0, 3) ;
        ss >> a(1, 0) >> a(1, 1) >> a(1, 2) >> a(1, 3) ;
        ss >> a(2, 0) >> a(2, 1) >> a(2, 2) >> a(2, 3) ;
        ss >> a(3, 0) >> a(3, 1) >> a(3, 2) >> a(3, 3) ;

        pose[bname] = a ;
    }

    return true ;
}

void update_node_transforms(NodePtr &scene, const MHX2Model &model, const Pose &pose) {
    for( const auto &mp: pose ) {
        const string &name = mp.first ;
        NodePtr node = scene->findNodeByName(name) ;
        auto it = model.bones_.find(name) ;
        const MHX2Bone &b = it->second ;
        if ( node ) {
            node->setTransform(node->transform() * mp.second ) ;
        }
    }
}


using namespace xviz ;

int main(int argc, char *argv[]) {

    TestApplication app("test_mh", argc, argv) ;

    Mhx2Importer importer ;
    importer.load("/home/malasiot/Downloads/human-cmu.mhx2", "Human_cmu:Body", true) ;

    const auto &mh = importer.getModel() ;
    NodePtr model(new MHNode(mh)) ;

    Matrix4f rot ;
    rot <<  1, 0, 0, 0,
            0, 0, -1, 0,
            0, 1, 0, 0,
            0, 0, 0, 1 ;




    //auto m = model->findNodeByName("LeftShoulder")->transform() ;

    auto m = importer.getModel().getBone("LeftShoulder")->bmat_ ;
    auto head = importer.getModel().getBone("LeftShoulder")->head_ ;


    cout << m << endl ;


/*
LeftShoulder
nmat
<Matrix 4x4 (-0.1831,  0.9826,  0.0320,  0.0203)
            (-0.1750, -0.0006, -0.9846, -0.0191)
            (-0.9674, -0.1858,  0.1721,  1.2164)
            ( 0.0000,  0.0000,  0.0000,  1.0000)>
mat
<Matrix 4x4 (-0.1831,  0.9826, 0.0320, 0.0203)
            (-0.9674, -0.1858, 0.1721, 0.3981)
            ( 0.1750,  0.0006, 0.9846, 0.0191)
            ( 0.0000,  0.0000, 0.0000, 1.0000)>
eb.mat
<Matrix 4x4 (-0.1831,  0.9826,  0.0320,  0.0203)
            (-0.1750, -0.0006, -0.9846, -0.0191)
            (-0.9674, -0.1858,  0.1721,  1.2164)
            ( 0.0000,  0.0000,  0.0000,  1.0000)>






 */
/*
    Pose pose ;
    load_pose_from_mhp("/home/malasiot/tmp/01_01_001.pose", pose) ;

    //model->updateTransforms(pose) ;

    update_node_transforms(model, mh, pose) ;
*/
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
