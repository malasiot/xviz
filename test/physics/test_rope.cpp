#include <xviz/scene/material.hpp>

#include <QApplication>
#include <QMainWindow>

#include <xviz/scene/camera.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/scene/scene.hpp>
#include <xviz/scene/node.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/node_helpers.hpp>
#include <xviz/physics/world.hpp>

#include <iostream>

#include "bullet_gui.hpp"

using namespace Eigen ;

using namespace std ;
using namespace xviz ;

class GUI: public SimulationGui {
public:
    GUI(ScenePtr scene, xviz::PhysicsWorld &physics, vector<RigidBodyPtr> &objects):
    SimulationGui(scene, physics), objects_(std::move(objects)) {

    }

    void onUpdate(float delta) override {
        SimulationGui::onUpdate(delta) ;

        vector<ContactResult> contacts ;
        if ( physics_.contactTest(objects_[0], contacts ) ) {
            for( ContactResult &c: contacts ) {

                    cout << c.a_->getName() << ' ' << c.b_->getName() << endl ;
            }
        }

    }

private:
    vector<RigidBodyPtr> objects_ ;
};

static PhysicsWorld physics ;
static ScenePtr scene ;

static const float chain_radius = 0.1 ;
static const float chain_length = 0.3 ;
static const int TOTAL_BOXES = 7;


static Vector4f sColors[4] =  {
    Vector4f(60. / 256., 186. / 256., 84. / 256., 1),
    Vector4f(244. / 256., 194. / 256., 13. / 256., 1),
    Vector4f(219. / 256., 50. / 256., 54. / 256., 1),
    Vector4f(72. / 256., 133. / 256., 237. / 256., 1),

};

vector<RigidBodyPtr> boxes;

// create cylinder aligned with Y axis
NodePtr makeCylinder(float radius, float length, const Eigen::Affine3f &tr, const Eigen::Vector4f &clr) {

    // we need an extra node to perform rotation of cylinder so that it is aligned with Y axis instead of Z

    auto node = NodeHelpers::makeCylinder(radius, length, clr) ;

    node->transform().rotate(Eigen::AngleAxisf(-0.5*M_PI, Eigen::Vector3f::UnitX()));

    NodePtr externalNode(new Node) ;
    externalNode->setTransform(tr) ;
    externalNode->addChild(node) ;

    return externalNode ;
}

void createScene() {
    scene.reset(new Scene) ;

    // init physics

    physics.createDefaultDynamicsWorld();

    // add light
    std::shared_ptr<DirectionalLight> dl( new DirectionalLight(Vector3f(0.5, 0.5, 1)) ) ;
    dl->diffuse_color_ = Vector3f(1, 1, 1) ;
    scene->addLightNode(dl) ;

    // create ground plane object

    Affine3f tr(Translation3f{0, -0.04, 0}) ;

    Vector3f ground_hs{10., 0.04, 10.} ;
    auto groundNode = NodeHelpers::makeBox(ground_hs, {0.5, 0.5, 0.5, 1}) ;
    groundNode->setTransform(tr) ;
    groundNode->setName("ground") ;
    scene->addChild(groundNode) ;
    RigidBodyPtr ground = make_shared<RigidBody>(CollisionShapePtr(new BoxCollisionShape(ground_hs)), tr);
    ground->setName("ground") ;
    physics.addRigidBody(ground) ;

    // create static pole
 //   Affine3f poleTransform(Translation3f{0.5, 5, 0}) ;
 //   scene->addCylinder(0.25, 10, poleTransform.matrix(), {0, 1, 0, 1})->setName("pole") ;
 //   RigidBody pole(CylinderCollisionShape(0.25, 10), poleTransform);
 //   pole.setName("pole") ;
 //   physics.addBody(pole) ;

    // create static box from mesh
    Affine3f meshTransform(Translation3f{2.5, 0.5, 1.5}) ;

    NodePtr meshNode(new Node) ;
    meshNode->load("/home/malasiot/Downloads/CoffeeTable.obj", 0) ;
    meshNode->transform().scale(3.5);
    NodePtr meshContainer(new Node) ;
    meshContainer->setTransform(meshTransform);
    meshContainer->addChild(meshNode) ;
    scene->addChild(meshContainer) ;

    CollisionShapePtr meshColShape(new StaticMeshCollisionShape("/home/malasiot/Downloads/CoffeeTable.obj")) ;
    meshColShape->setLocalScale(3.5);

    RigidBodyPtr mesh = make_shared<RigidBody>(1.0, new UpdateSceneMotionState(meshContainer), meshColShape);
    mesh->setName("mesh") ;
    physics.addRigidBody(mesh) ;

    // create collision shape for chain element

    btScalar mass(1.0) ;

    CollisionShapePtr colShape(new CylinderCollisionShape(chain_radius, chain_length)) ;

    int lastBoxIndex = TOTAL_BOXES - 1;

    for (int i = 0; i < TOTAL_BOXES; i++) {
        float tx = 0, ty = 2.0f+ i*1.5*chain_length, tz = 0 ;

        Affine3f box_tr(Translation3f{tx, ty, tz}) ;

        NodePtr chain_node = makeCylinder(chain_radius, chain_length, box_tr, sColors[i%4]) ;
        stringstream name ;
        name << "chain " << i ;
        chain_node->setName(name.str()) ;

        scene->addChild(chain_node) ;

        if ( i== lastBoxIndex ) {
            RigidBodyPtr box(new RigidBody(colShape, box_tr)) ;
            box->setName(name.str()) ;
            physics.addRigidBody(box) ;
            boxes.push_back(box) ;
        }
        else {
            RigidBodyPtr box(new RigidBody(mass, new UpdateSceneMotionState(chain_node), colShape)) ;
            box->setName(name.str()) ;
            physics.addRigidBody(box) ;
            boxes.push_back(box) ;
        }

    }
    //add N-1 spring constraints
    for (int i = 0; i < TOTAL_BOXES - 1; ++i) {
        Point2PointConstraint c(boxes[i], boxes[i+1], {0.0, 1.5*chain_length/2.0, 0}, {0.0, -1.5*chain_length/2.0, 0}) ;
        physics.addConstraint(c);
    }

}

int main(int argc, char **argv)
{
    createScene() ;

    QApplication app(argc, argv);

    SceneViewer::initDefaultGLContext();

    QMainWindow window ;
    window.setCentralWidget(new GUI(scene, physics, boxes)) ;
    window.resize(1024, 1024) ;
    window.show() ;

    return app.exec();
}
