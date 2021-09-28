#ifndef XVIZ_GUI_MANIPULATOR_HPP
#define XVIZ_GUI_MANIPULATOR_HPP

#include <xviz/scene/node.hpp>
#include <xviz/scene/raycaster.hpp>

class QMouseEvent ;

namespace xviz {

class Manipulator ;
using ManipulatorPtr = std::shared_ptr<Manipulator> ;

class Manipulator: public Node {
public:

    Manipulator() = default ;

    void setCamera(const CameraPtr &cam) ;

    virtual bool onMousePressed(QMouseEvent *event) = 0 ;
    virtual bool onMouseReleased(QMouseEvent *event) = 0;
    virtual bool onMouseMoved(QMouseEvent *event) = 0 ;

protected:
    friend class CompositeManipulator ;

    CameraPtr camera_ ;
    Manipulator *container_ = nullptr ;
};

class CompositeManipulator: public Manipulator {
public:
    bool onMousePressed(QMouseEvent *event) override ;
    bool onMouseReleased(QMouseEvent *event) override ;
    bool onMouseMoved(QMouseEvent *event) override ;

protected:
    void addComponent(const ManipulatorPtr &m) ;

    std::vector<ManipulatorPtr> components_ ;
};

class Translate1DManipulator: public Manipulator {
public:
    Translate1DManipulator(const Eigen::Vector3f &start, const Eigen::Vector3f &end) ;

    void setColor(const Eigen::Vector4f &clr) ;
    void setPickColor(const Eigen::Vector4f &clr) ;
    void setPickThreshold(float t) ;

    bool onMousePressed(QMouseEvent *event) override ;
    bool onMouseReleased(QMouseEvent *event) override ;
    bool onMouseMoved(QMouseEvent *event) override ;

private:
    Eigen::Vector3f start_, end_, start_drag_, end_drag_, translation_init_ ;
    Eigen::Vector4f clr_{1, 0, 0, 1}, pick_clr_{1, 1, 0, 1} ;
    GeometryPtr cone_, line_ ;
    MaterialPtr mat_ ;
    bool dragging_ = false ;
    float pick_threshold_  ;

    void setMaterialColor(const Eigen::Vector4f &clr);
};

class TranslateXYZManipulator: public CompositeManipulator {
public:
    TranslateXYZManipulator(float line_width) ;

private:
    Translate1DManipulator mx_, my_, mz_ ;
};


}

#endif
