#include <xviz/gui/viewer.hpp>
#include <xviz/gui/translation_manipulator.hpp>
#include <xviz/gui/rotation_manipulator.hpp>

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
    SceneViewer::initDefaultGLContext();

    NodePtr scene(new Node) ;

    NodePtr box_node(new Node) ;
    GeometryPtr geom(new BoxGeometry({1, 2, 1})) ;
    PhongMaterial *material = new PhongMaterial({1, 0, 1, 0.1}) ;
    MaterialPtr mat(material) ;
    box_node->addDrawable(geom, mat) ;
    scene->addChild(box_node) ;


    RotateXYZManipulator *rmanip = new RotateXYZManipulator(box_node, 1.f);
    TranslateXYZManipulator *tmanip = new TranslateXYZManipulator(box_node, 2.f);

    rmanip->setVisible(false) ;
    rmanip->setOrder(2) ;
    tmanip->setOrder(2) ;

    ManipulatorPtr rm(rmanip) ;
    ManipulatorPtr tm(tmanip) ;

    box_node->addChild(rm) ;
    box_node->addChild(tm) ;

    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->diffuse_color_ = Vector3f(1, 1, 1) ;
    scene->addLightNode(LightPtr(dl)) ;

    SceneViewer *viewer = new SceneViewer(scene) ;
    viewer->initCamera({0, 0, 0}, 4.0);
    viewer->addManipulator(rm) ;
    viewer->addManipulator(tm) ;

    QMainWindow window ;
    window.setCentralWidget(viewer) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
