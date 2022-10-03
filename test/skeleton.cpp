#include <xviz/gui/viewer.hpp>
#include <xviz/gui/offscreen.hpp>
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

        if ( n.name() == "human_cmu-baseObject" ) {

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

void set_geom_visibility(NodePtr node, bool visible) {
    node->visit([&](Node &n) {
        if ( n.name() != "human_cmu-baseObject" ) {
            if ( !n.drawables().empty() ) {
                cout << n.name() << endl ;
                n.setVisible(visible, true) ;
            }
        }
    });
}

int main(int argc, char **argv)
{
    TestApplication app("skeleton", argc, argv) ;

    ScenePtr model(new Scene) ;

  //  model->load(TestApplication::data() + "/models/RiggedFigure.dae", Node::IMPORT_ANIMATIONS | Node::IMPORT_SKELETONS) ;

    model->load( "/home/malasiot/source/human_tracking/pose_estimation/data/models/human-cmu.dae", Node::IMPORT_ANIMATIONS | Node::IMPORT_SKELETONS) ;
    replace_texture(model, "/home/malasiot/source/human_tracking/pose_estimation/data/parts.png") ;
    set_geom_visibility(model, false) ;

    float r = 7 ;
    Vector3f c{0, 0.75, 0.0};
    unsigned int width = 480, height = 480 ;
    PerspectiveCamera *pcam = new PerspectiveCamera(1, // aspect ratio
                                                    50*M_PI/180,   // fov
                                                    0.0001,        // zmin
                                                    10*r           // zmax
                                                    ) ;


//    OrthographicCamera *pcam = new OrthographicCamera(-0.6*r, 0.6*r, 0.6*r, -0.6*r,0.0001, 10*r) ;

    CameraPtr cam(pcam) ;

    cam->setBgColor({1, 0, 0, 1});

    // position camera to look at the center of the object

  //  pcam->viewSphere(c, r) ;
    pcam->lookAt(c + Vector3f{0, 0, 4*r}, c, {0, 1, 0}) ;

    // set camera viewpot

    pcam->setViewport(width, height)  ;

    OffscreenRenderer rdr(QSize(width, height));
    rdr.render(model, cam) ;
    rdr.getImage().save("/tmp/clr.png");
    rdr.getDepthBuffer(0.0001, 10*r).save("/tmp/depth.png") ;
/*
    SceneViewer::initDefaultGLContext();
    SceneViewer *viewer = new SceneViewer(model);
    viewer->setDefaultCamera() ;
    viewer->startAnimations() ;

    QMainWindow window ;
    window.setCentralWidget(viewer) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
    */
}
