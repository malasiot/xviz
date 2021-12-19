#ifndef XVIZ_GUI_ROTATION_MANIPULATOR_HPP
#define XVIZ_GUI_ROTATION_MANIPULATOR_HPP

#include <xviz/gui/manipulator.hpp>

namespace xviz {

class RotateAxisManipulator: public Manipulator {
public:
    RotateAxisManipulator(const NodePtr &node, const Eigen::Vector3f &axis, float radius) ;

    void setColor(const Eigen::Vector4f &clr) ;
    void setPickColor(const Eigen::Vector4f &clr) ;

    bool onMousePressed(QMouseEvent *event) override ;
    bool onMouseReleased(QMouseEvent *event) override ;
    bool onMouseMoved(QMouseEvent *event) override ;
    void onCameraUpdated() override ;

private:
    Eigen::Vector3f axis_, start_drag_, end_drag_ ;
    float radius_ ;
    Eigen::Matrix3f rotation_init_ ;
    Eigen::Affine3f tr_init_ ;
    Eigen::Vector4f clr_{1, 0, 0, 1}, pick_clr_{1, 1, 0, 1} ;
    MaterialPtr mat_ ;
    NodePtr scale_node_ ;
    bool dragging_ = false ;
    float pick_threshold_  ;

    void setMaterialColor(const Eigen::Vector4f &clr);
};

class RotateXYZManipulator: public CompositeManipulator {
public:
    RotateXYZManipulator(const NodePtr &node, float radius) ;

private:
    ManipulatorPtr mx_, my_, mz_ ;
};


}

#endif
