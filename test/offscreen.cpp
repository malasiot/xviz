#include <xviz/gui/offscreen.hpp>
#include <xviz/scene/renderer.hpp>
#include <xviz/scene/scene.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/camera.hpp>
#include <xviz/scene/geometry.hpp>

#include <QImage>
#include <QApplication>

#include "util.hpp"

using namespace xviz ;
using namespace Eigen ;

int main(int argc, char *argv[]) {
    TestApplication app("offscreen", argc, argv);

      unsigned int width = 480, height = 480 ;
     OffscreenSurface os(QSize(width, height));

    ScenePtr scene(new Scene) ;
   // scene->load("/home/malasiot/Downloads/greek_column.obj") ;
    scene->load(TestApplication::data() + "/models/cube.obj") ;
 //   scene->load("/home/malasiot/Downloads/human.dae") ;

    // optional compute center and radius to properly position camera
    auto c = scene->geomCenter() ;
    auto r = scene->geomRadius(c) ;


    NodePtr box(new Node) ;
    GeometryPtr box_geom(new Geometry(Geometry::createSolidCube({0.1f, 0.1f, 0.1f})));
    MaterialPtr mat(new WireFrameMaterial({1, 0, 0, 1}, {0, 1, 1, 1})) ;
    box->addDrawable(box_geom, mat) ;
    scene->addChild(box) ;

    // add a light source

    DirectionalLight *dl = new DirectionalLight(Vector3f(1, 1, 1)) ;
    dl->setDiffuseColor(Vector3f(1, 1, 1)) ;
    scene->setLight(LightPtr(dl)) ;

    // create a camera

    PerspectiveCamera *pcam = new PerspectiveCamera(1, // aspect ratio
                                                    50*M_PI/180,   // fov
                                                    0.01,        // zmin
                                                    10*r           // zmax
                                                    ) ;


 //   OrthographicCamera *pcam = new OrthographicCamera(-0.6*r, 0.6*r, 0.6*r, -0.6*r,0.0001, 10*r) ;

    CameraPtr cam(pcam) ;

    cam->setBgColor({1, 0, 0, 1});

    // position camera to look at the center of the object

  //  pcam->viewSphere(c, r) ;
    pcam->lookAt({1, 1, 5}, {0, 0, 0}, {0, 1, 0}) ;

    // set camera viewpot

    pcam->setViewport(width, height)  ;


    Renderer rdr ;

    rdr.render(scene, cam) ;
    auto im = os.getImage() ;
    im.saveToPNG("/tmp/im.png") ;
}
