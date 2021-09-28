#ifndef XVIZ_GUI_SCALING_MANIPULATOR_HPP
#define XVIZ_GUI_SCALING_MANIPULATOR_HPP

#include <xviz/gui/manipulator.hpp>

namespace xviz {

class Scale1DManipulator: public Manipulator {
public:
    Scale1DManipulator(const NodePtr &node, const Eigen::Vector3f &start, const Eigen::Vector3f &end) ;

    void setColor(const Eigen::Vector4f &clr) ;
    void setPickColor(const Eigen::Vector4f &clr) ;
    void setPickThreshold(float t) ;

    bool onMousePressed(QMouseEvent *event) override ;
    bool onMouseReleased(QMouseEvent *event) override ;
    bool onMouseMoved(QMouseEvent *event) override ;

private:
    Eigen::Vector3f start_, end_, start_drag_, end_drag_, scaling_init_ ;
    Eigen::Vector4f clr_{1, 0, 0, 1}, pick_clr_{1, 1, 0, 1} ;
    MaterialPtr mat_ ;
    bool dragging_ = false ;
    float pick_threshold_ = 0.05 ;

    void setMaterialColor(const Eigen::Vector4f &clr);
};

class ScaleXYZManipulator: public CompositeManipulator {
public:
    ScaleXYZManipulator(const NodePtr &node, float line_width) ;

private:
    ManipulatorPtr mx_, my_, mz_ ;
};


}

#endif
