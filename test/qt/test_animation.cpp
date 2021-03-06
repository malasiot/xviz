#include <xviz/qt/scene/viewer.hpp>

#include <xviz/scene/scene.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/qt/resource_loader.hpp>

#include <xviz/scene/node_animation.hpp>

#include <random>
#include <iostream>

#include <QMainWindow>
#include <QApplication>

using namespace xviz ;
using namespace Eigen ;
using namespace std ;

int main(int argc, char **argv)
{
    ScenePtr model(new Scene) ;

    model->load("/home/malasiot/Downloads/BoxAnimated.dae", Node::IMPORT_ANIMATIONS) ;

    NodePtr box(new Node) ;
    GeometryPtr geom(new BoxGeometry({0.5, 0.5, 0.5})) ;
    MaterialPtr mat(new PhongMaterial({1, 0, 0, 1}));
    box->addDrawable(geom, mat) ;
    model->addChild(box) ;

    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->diffuse_color_ = Vector3f(1, 1, 1) ;
    model->addLightNode(LightPtr(dl)) ;

    NodeAnimation *anim = new NodeAnimation(box) ;
    anim->addKeyFrame(0.0, {0.0f, 2.0f, 1.0f}, {1.0, 0.0f, 0.0f, 0.0f}, {1, 1, 1});
    anim->addKeyFrame(1.0, {4.0f, 2.0f, 1.0f}, {1.0, 0.5f, 0.0f, 0.0f}, {0.5, 0.5, 0.5});
    anim->setDuration(2500) ;

    model->addAnimation(anim) ;

    model->startAnimations(0);

    QApplication app(argc, argv);

    SceneViewer::initDefaultGLContext();
    SceneViewer *viewer = new SceneViewer(model);
    viewer->startAnimations() ;

    QMainWindow window ;
    window.setCentralWidget(viewer) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
