#include "sim_gui.hpp"
#include <clsim/scene/scene.hpp>
#include <clsim/physics/world.hpp>
#include <QTimer>

using namespace clsim ;

SimulationGui::SimulationGui(World &physics): SceneViewer(physics.getVisual()), physics_(physics) {

    auto scene = physics.getVisual() ;

    auto c = scene->geomCenter();
    initCamera(c, scene->geomRadius(c)) ;

    camera_->setBgColor({1, 1, 1, 1}) ;

    startAnimations() ;
}

SimulationGui::SimulationGui(World &physics, const Eigen::Vector3f &c, float r): SceneViewer(physics.getVisual()), physics_(physics) {
    initCamera(c, r) ;

    camera_->setBgColor({1, 1, 1, 1}) ;

    startAnimations() ;
}

void SimulationGui::mousePressEvent(QMouseEvent *event)
{
    if ( event->modifiers() & Qt::ControlModifier ) {
        Ray ray = camera_->getRay(event->x(), event->y()) ;

        picking_ = true ;

    } else {
        SceneViewer::mousePressEvent(event) ;
    }


}

void SimulationGui::mouseReleaseEvent(QMouseEvent *event)
{
    if ( event->modifiers() & Qt::ControlModifier ) {

        picking_ = false ;
    } else {
        SceneViewer::mouseReleaseEvent(event) ;
    }

}

void SimulationGui::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->x() ;
    int y = event->y() ;

    if ( picking_ )  {
        Ray ray = camera_->getRay(x, y) ;


        update() ;
    }
    else {
        SceneViewer::mouseMoveEvent(event) ;

    }

}

void SimulationGui::onUpdate(float delta) {
    physics_.stepSimulation(delta/1000.0f);
    //  physics_.stepSimulation(0.0001);

}

