#include <xviz/qt/scene/viewer.hpp>

#include <xviz/scene/scene.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/node_helpers.hpp>

#include <xviz/qt/resource_loader.hpp>

#include <QMainWindow>
#include <QApplication>

using namespace xviz ;
using namespace Eigen ;


int main(int argc, char **argv)
{
    ScenePtr scene(new Scene) ;

    ResourceLoader::instance().setLocalPath("/home/malasiot/Downloads/");
    // scene->load("/home/malasiot/Downloads/greek_column.obj", nullptr, true) ;
     scene->load("/home/malasiot/Downloads/cube.obj") ;


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
