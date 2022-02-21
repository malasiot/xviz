#include <xviz/gui/viewer.hpp>

#include <xviz/scene/scene.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/node_helpers.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/material.hpp>

#include <QMainWindow>
#include <QApplication>

#include "util.hpp"

using namespace xviz ;
using namespace Eigen ;


int main(int argc, char **argv)
{
    TestApplication app("renderer", argc, argv) ;

    ScenePtr scene(new Scene) ;

    scene->load(TestApplication::data() + "models/capsule.obj") ;

    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->setDiffuseColor(Vector3f(0.5, 0.5, 0.5)) ;
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


    SceneViewer::initDefaultGLContext();

    SceneViewer *viewer = new SceneViewer(scene) ;
    viewer->setDefaultCamera() ;
    viewer->makeAxes(1.5) ;
    viewer->setDrawAxes(true);
    QMainWindow window ;
    window.setCentralWidget(viewer) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
