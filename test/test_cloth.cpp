#include <xviz/scene/material.hpp>

#include <QApplication>
#include <QMainWindow>

#include <xviz/scene/camera.hpp>
#include <xviz/scene/light.hpp>

#include <xviz/scene/node.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/mesh.hpp>

#include <iostream>

#include "physics/world.hpp"
#include "physics/cloth.hpp"
#include "physics/solver.hpp"
#include "physics/collision_object.hpp"

#include <xviz/scene/scene.hpp>
#include <xviz/scene/node_helpers.hpp>

#include <xviz/gui/viewer.hpp>
#include "sim_gui.hpp"
#include "util.hpp"

using namespace Eigen ;

using namespace std ;
using namespace xviz ;

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
                                                   10.5, 0.001);

    physics.setCloth(cloth) ;

    Isometry3f mat ;
    mat.setIdentity() ;
    mat.translate(Vector3f{0, 0., 0.2}) ;
    physics.addCollisionObject(new CollisionObject("/home/malasiot/Downloads/cube.obj", mat)) ;
    solver = new Solver(physics) ;

    physics.setSolver(solver) ;

    physics.resetSimulation() ;

}


int main(int argc, char **argv)
{

    createWorld() ;

    TestApplication app("cloth", argc, argv);

    SceneViewer::initDefaultGLContext();

    QMainWindow window ;
    window.setCentralWidget(new SimulationGui(physics, { 0, 0, 0}, 3.0)) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
