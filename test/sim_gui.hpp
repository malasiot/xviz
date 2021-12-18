#pragma once

#include <xviz/scene/scene_fwd.hpp>
#include <xviz/gui/viewer.hpp>

#include "physics/solver.hpp"

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QElapsedTimer>

#include <iostream>

class SimulationGui : public xviz::SceneViewer
{
public:
    SimulationGui(xviz::World &physics) ;
    SimulationGui(xviz::World &physics, const Eigen::Vector3f &c, float r);
protected:

    xviz::World &physics_ ;

    bool picking_ = false ;


protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    virtual void onUpdate(float delta) override;
};
