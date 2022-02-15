#include <xviz/gui/offscreen.hpp>
#include <xviz/scene/scene.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/camera.hpp>

#include <QImage>
#include <QApplication>

using namespace xviz ;
using namespace Eigen ;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ScenePtr scene(new Scene) ;
   // scene->load("/home/malasiot/Downloads/greek_column.obj") ;
    scene->load("/home/malasiot/Downloads/cube.obj") ;

    // optional compute center and radius to properly position camera
    auto c = scene->geomCenter() ;
    auto r = scene->geomRadius(c) ;

    // add a ligh source

    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->setDiffuseColor(Vector3f(1, 1, 1)) ;
    scene->addLightNode(LightPtr(dl)) ;

    // create a camera
    unsigned int width = 480, height = 480 ;
  /*  PerspectiveCamera *pcam = new PerspectiveCamera(1, // aspect ratio
                                                    50*M_PI/180,   // fov
                                                    0.0001,        // zmin
                                                    10*r           // zmax
                                                    ) ;
*/

    OrthographicCamera *pcam = new OrthographicCamera(0.6*r, 0.6*r, 0.0001, 10*r) ;

    CameraPtr cam(pcam) ;

    cam->setBgColor({1, 0, 0, 1});

    // position camera to look at the center of the object

  //  pcam->viewSphere(c, r) ;
    pcam->lookAt(c + Vector3f{0, 0, 2*r}, c, {0, 1, 0}) ;

    // set camera viewpot

    pcam->setViewport(width, height)  ;

    OffscreenRenderer rdr(nullptr, QSize(width, height));
    rdr.render(scene, cam).save("/tmp/oo.png");
}
