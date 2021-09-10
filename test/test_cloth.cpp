#include <clsim/scene/material.hpp>

#include <QApplication>
#include <QMainWindow>

#include <clsim/scene/camera.hpp>
#include <clsim/scene/light.hpp>

#include <clsim/scene/node.hpp>
#include <clsim/scene/geometry.hpp>
#include <clsim/scene/mesh.hpp>

#include <iostream>

#include <clsim/physics/world.hpp>
#include <clsim/physics/cloth.hpp>
#include <clsim/physics/solver.hpp>

#include <clsim/scene/scene.hpp>
#include <clsim/scene/node_helpers.hpp>

#include <clsim/gui/viewer.hpp>
#include "sim_gui.hpp"

using namespace Eigen ;

using namespace std ;
using namespace clsim ;

static World physics ;
static Solver *solver ;
static NodePtr cloth_node ;

void createWorld() {
    // init physics

    const int NUM_X = 21 ;
    const int NUM_Z = 21 ;

    RectangularPatch *cloth = new RectangularPatch(0.1, {-2, 3, 0}, {2, 3, 0}, {-2, 3, 4},
                                                   NUM_X, NUM_Z,
                                                   RectangularPatch::TopLeft | RectangularPatch::TopRight,
                                                   1110.5, 0.001);

    physics.setCloth(cloth) ;
    solver = new Solver(physics) ;

    physics.setSolver(solver) ;

    physics.resetSimulation() ;

}


int main(int argc, char **argv)
{

    createWorld() ;

    QApplication app(argc, argv);

    SceneViewer::initDefaultGLContext();

    QMainWindow window ;
    window.setCentralWidget(new SimulationGui(physics, { 0, 0, 0}, 3.0)) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
