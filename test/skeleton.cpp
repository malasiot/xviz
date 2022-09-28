#include <xviz/gui/viewer.hpp>

#include <xviz/scene/scene.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/light.hpp>

#include <xviz/scene/node_animation.hpp>

#include <random>
#include <iostream>

#include <QMainWindow>
#include <QApplication>

#include "util.hpp"

using namespace xviz ;
using namespace Eigen ;
using namespace std ;

static void replace_texture(NodePtr node, const string &tpath) {
    node->visit([&](Node &n) {
        cout << n.name() << endl ;

        if ( n.name() == "human-baseObject" ) {

            MaterialPtr mat = n.drawables()[0].material() ;

            ConstantMaterial *cmat = new ConstantMaterial({0, 0, 0, 1}) ;

           ImagePtr image(new Image(tpath)) ;
           Sampler2D sampler(Sampler2D::WRAP_CLAMP, Sampler2D::WRAP_CLAMP) ;
           sampler.setMagnification(Sampler2D::MAG_NEAREST) ;
           sampler.setMinification(Sampler2D::MIN_NEAREST_MIPMAP_NEAREST) ;

           Texture2D *texture = new Texture2D(image, Sampler2D()) ;
           cmat->setTexture(texture) ;
           n.drawables()[0].setMaterial(MaterialPtr(cmat)) ;

        }
    });
}

int main(int argc, char **argv)
{
    TestApplication app("skeleton", argc, argv) ;

    ScenePtr model(new Scene) ;

  //  model->load(TestApplication::data() + "/models/RiggedFigure.dae", Node::IMPORT_ANIMATIONS | Node::IMPORT_SKELETONS) ;

    model->load( "/home/malasiot/Downloads/human.dae", Node::IMPORT_ANIMATIONS | Node::IMPORT_SKELETONS) ;
    replace_texture(model, "/home/malasiot/Downloads/textures/parts_texture.png") ;

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
