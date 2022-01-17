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
    SceneViewer::initDefaultGLContext();

    NodePtr scene(new Node) ;

    NodePtr box_node(new Node) ;
    box_node->transform().translation() = Vector3f{0, 0.5f, 0} ;
    GeometryPtr geom(new BoxGeometry({1, 2, 1})) ;
    PhongMaterial *material = new PhongMaterial({1, 0, 1, 0.1}) ;
    MaterialPtr mat(material) ;
    box_node->addDrawable(geom, mat) ;
    scene->addChild(box_node) ;


    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->diffuse_color_ = Vector3f(1, 1, 1) ;
    scene->addLightNode(LightPtr(dl)) ;

    SceneViewer *viewer = new SceneViewer(scene) ;
    viewer->initCamera({0, 0, 0}, 4.0);

    TransformGizmo *gizmo = new TransformGizmo(viewer->getCamera(), 2.0) ;
    gizmo->setOrder(2) ;
    gizmo->attachTo(box_node.get()) ;

    std::shared_ptr<TransformGizmo> gz(gizmo) ;

    viewer->addManipulator(gizmo) ;
    scene->addChild(gz);


    QMainWindow window ;
    window.setCentralWidget(viewer) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
