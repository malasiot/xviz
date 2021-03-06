#include <xviz/qt/scene/offscreen.hpp>
#include <xviz/scene/scene.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/camera.hpp>

#include <QImage>
#include <QApplication>

#include <xviz/qt/resource_loader.hpp>

using namespace xviz ;
using namespace Eigen ;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

     ResourceLoader::instance().setLocalPath("/home/malasiot/Downloads/");

    ScenePtr scene(new Scene) ;
   // scene->load("/home/malasiot/Downloads/greek_column.obj") ;
    scene->load("/home/malasiot/Downloads/cube.obj") ;

    // optional compute center and radius to properly position camera
    auto c = scene->geomCenter() ;
    auto r = scene->geomRadius(c) ;

    // add a ligh source

    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->diffuse_color_ = Vector3f(1, 1, 1) ;
    scene->addLightNode(LightPtr(dl)) ;

    // create a camera
    uint width = 480, height = 480 ;
    PerspectiveCamera *pcam = new PerspectiveCamera(1, // aspect ratio
                                                    50*M_PI/180,   // fov
                                                    0.0001,        // zmin
                                                    10*r           // zmax
                                                    ) ;

    CameraPtr cam(pcam) ;

    cam->setBgColor({1, 0, 0, 1});

    // position camera to look at the center of the object

    pcam->lookAt(c + Vector3f{0.3, 0.8, 2*r}, c, {0, 1, 0}) ;

    // set camera viewpot

    pcam->setViewport(width, height)  ;

    QOffscreenRenderer rdr(nullptr, QSize(width, height));
    rdr.render(scene, cam).save("/tmp/oo.png");
}
