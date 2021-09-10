#pragma once

#include <clsim/scene/scene_fwd.hpp>
#include <clsim/gui/viewer.hpp>

#include <clsim/physics/solver.hpp>

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QElapsedTimer>

#include <iostream>

class SimulationGui : public clsim::SceneViewer
{
public:
    SimulationGui(clsim::World &physics) ;
    SimulationGui(clsim::World &physics, const Eigen::Vector3f &c, float r);
protected:

    clsim::World &physics_ ;

    bool picking_ = false ;


protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    virtual void onUpdate(float delta) override;
};
