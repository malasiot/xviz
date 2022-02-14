#include <xviz/gui/viewer.hpp>

#include <xviz/scene/scene.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/light.hpp>

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

    model->load("/home/malasiot/Downloads/rp_nathan_animated_003_walking.fbx", Node::IMPORT_ANIMATIONS | Node::IMPORT_SKELETONS ) ;

    NodePtr box(new Node) ;
    GeometryPtr geom(new BoxGeometry({15, 15, 15})) ;
    MaterialPtr mat(new PhongMaterial(Vector4f{1, 0, 0, 1}));
    box->addDrawable(geom, mat) ;
    model->addChild(box) ;

    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->diffuse_color_ = Vector3f(1, 1, 1) ;
    model->addLightNode(LightPtr(dl)) ;

    NodeAnimation *anim = new NodeAnimation(box) ;
    anim->addKeyFrame(0.0, {0.0f, 200.0f, 100.0f}, {1.0, 0.0f, 0.0f, 0.0f}, {1, 1, 1});
    anim->addKeyFrame(1.0, {400.0f, 200.0f, 100.0f}, {1.0, 0.5f, 0.0f, 0.0f}, {0.5, 0.5, 0.5});
    anim->setDuration(6400) ;

    model->addAnimation(anim) ;

    model->startAnimations(0);

    QApplication app(argc, argv);

    SceneViewer::initDefaultGLContext();
    SceneViewer *viewer = new SceneViewer(model);
    viewer->setDefaultCamera() ;
    viewer->startAnimations() ;

    QMainWindow window ;
    window.setCentralWidget(viewer) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
