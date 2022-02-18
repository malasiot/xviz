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

int main(int argc, char **argv)
{
    TestApplication app("skeleton", argc, argv) ;

    ScenePtr model(new Scene) ;

    model->load(TestApplication::data() + "/models/RiggedFigure.dae", Node::IMPORT_ANIMATIONS | Node::IMPORT_SKELETONS) ;

    model->startAnimations(0);

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
