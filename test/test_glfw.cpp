#include <xviz/gui/viewer_glfw.hpp>
#include <xviz/scene/scene.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/node_helpers.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/material.hpp>

using namespace xviz ;
using namespace Eigen ;

int main(int argc, char *argv[]) {
    NodePtr scene(new Node) ;

    scene->load("models/capsule.obj") ;

    NodePtr model(new Node) ;
    model->load("models/2CylinderEngine.glb");
    Affine3f tr2(Affine3f::Identity());
 //   tr2.scale(0.001f) ;
    tr2.translate(-model->geomCenter()) ;
    model->setTransform(tr2) ;
    model->setName("model") ;

    scene->addChild(model);

    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->setDiffuseColor(Vector3f(1, 1, 1)) ;
    dl->setSpecularColor(Vector3f(0.5, 0.5, 0.5)) ;
    dl->setAmbientColor(Vector3f(0.5, 0.5, 0.5)) ;

    scene->setLight(LightPtr(dl)) ;

    NodePtr box(new Node) ;
    GeometryPtr box_geom(new Geometry(Geometry::createSolidCube({0.1f, 0.1f, 0.1f})));
    MaterialPtr mat(new WireFrameMaterial({1, 0, 0, 1}, {0, 1, 1, 1})) ;
    box->addDrawable(box_geom, mat) ;

    Isometry3f tr = Isometry3f::Identity() ;
    tr.translate(Vector3f{0.5, -1.0, 0}) ;
    box->setTransform(tr) ;
    scene->addChild(box) ;



    SceneViewerGLFW *viewer = new SceneViewerGLFW(scene) ;
    viewer->setDefaultCamera() ;
//    viewer->makeAxes(1.5) ;
//    viewer->setDrawAxes(true);

    auto cam = viewer->getCamera() ;


    viewer->runLoop() ;



}
