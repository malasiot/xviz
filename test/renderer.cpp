#include <clsim/gui/viewer.hpp>

#include <clsim/scene/scene.hpp>
#include <clsim/scene/light.hpp>
#include <clsim/scene/node_helpers.hpp>


#include <QMainWindow>
#include <QApplication>

using namespace clsim ;
using namespace Eigen ;


int main(int argc, char **argv)
{
    ScenePtr scene(new Scene) ;

    scene->load("/home/malasiot/Downloads/capsule.obj") ;

    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->diffuse_color_ = Vector3f(0.5, 0.5, 0.5) ;
    scene->setLight(LightPtr(dl)) ;

    QApplication app(argc, argv);
    SceneViewer::initDefaultGLContext();

    SceneViewer *viewer = new SceneViewer(scene) ;
    viewer->setDrawAxes(true);
    QMainWindow window ;
    window.setCentralWidget(viewer) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
