#pragma once

#include <cvx/viz/scene/scene_fwd.hpp>
#include <xviz/qt/scene/viewer.hpp>

#include <xviz/physics/world.hpp>

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QElapsedTimer>

#include <iostream>

class SimulationGui : public SceneViewer
{
public:
    SimulationGui(xviz::ScenePtr scene, xviz::PhysicsWorld &physics) ;
    SimulationGui(xviz::ScenePtr scene, xviz::PhysicsWorld &physics, const Eigen::Vector3f &c, float r);
protected:

    xviz::PhysicsWorld &physics_ ;
    xviz::RayPicker picker_ ;

    bool picking_ = false ;


protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    virtual void onUpdate(float delta) override {
        physics_.stepSimulation(delta/1000.0f);

    }
};
