#include <xviz/gui/viewer.hpp>
#include <xviz/gui/manipulator.hpp>

#include <xviz/scene/scene.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/light.hpp>

#include <random>
#include <iostream>

#include <QMainWindow>
#include <QApplication>
#include <QPainter>

using namespace xviz ;
using namespace Eigen ;
using namespace std ;


int main(int argc, char **argv)
{
     QApplication app(argc, argv);

    NodePtr box_node(new Node) ;

    GeometryPtr geom(new BoxGeometry({1, 2, 1})) ;

    ConstantMaterial *material = new ConstantMaterial({1, 0, 1, 1}) ;

    MaterialPtr mat(material) ;
    box_node->addDrawable(geom, mat) ;

    NodePtr scene(new Node) ;


    Translate1DManipulator *manip = new Translate1DManipulator({ -2.0, 0, 0}, {2, 0, 0});

    ManipulatorPtr m(manip) ;

    m->addChild(box_node) ;

    scene->addChild(m) ;

    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->diffuse_color_ = Vector3f(1, 1, 1) ;
    scene->addLightNode(LightPtr(dl)) ;


    SceneViewer *viewer = new SceneViewer(scene) ;
    viewer->addManipulator(m) ;



    SceneViewer::initDefaultGLContext();
    QMainWindow window ;
    window.setCentralWidget(viewer) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
