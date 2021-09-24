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

    model->load("/home/malasiot/Downloads/RiggedFigure.dae", Node::IMPORT_ANIMATIONS | Node::IMPORT_SKELETONS) ;

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
