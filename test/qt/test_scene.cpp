#include <xviz/qt/scene/viewer.hpp>

#include <xviz/scene/scene.hpp>
#include <xviz/scene/light.hpp>

#include <QMainWindow>
#include <QApplication>

using namespace xviz ;
using namespace Eigen ;


int main(int argc, char **argv)
{
    ScenePtr scene(new Scene) ;

    // scene->load("/home/malasiot/Downloads/greek_column.obj", nullptr, true) ;
     scene->load("/home/malasiot/Downloads/cube.obj") ;
  //  scene->load("/home/malasiot/Downloads/bunny.obj") ;
/*
    for( uint i=0 ; i<10 ; i++ ) {
        Vector4f clr(0.5, g_rng.uniform(0.0, 1.0), g_rng.uniform(0.0, 1.0), 1.0) ;

        scene->addChild(randomBox(format("box%d", i), Material::makeLambertian(clr), Vector3f(0.04, g_rng.uniform(0.1, 0.15), 0.04))) ;
    }
*/
    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->diffuse_color_ = Vector3f(1, 1, 1) ;
    scene->addLight(LightPtr(dl)) ;

    QApplication app(argc, argv);

    QSurfaceFormat format;
      format.setDepthBufferSize(24);
      format.setMajorVersion(3);
      format.setMinorVersion(3);

      format.setSamples(4);
      format.setProfile(QSurfaceFormat::CoreProfile);

      QSurfaceFormat::setDefaultFormat(format);

    QMainWindow window ;
    window.setCentralWidget(new SceneViewer(scene)) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
