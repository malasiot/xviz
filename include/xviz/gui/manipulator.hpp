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

    Manipulator(const NodePtr &node): transform_node_(node) {}

    void setCamera(const CameraPtr &cam) ;

    virtual bool onMousePressed(QMouseEvent *event) = 0 ;
    virtual bool onMouseReleased(QMouseEvent *event) = 0;
    virtual bool onMouseMoved(QMouseEvent *event) = 0 ;

protected:
    friend class CompositeManipulator ;

    CameraPtr camera_ ;
    NodePtr transform_node_ ;
};

class CompositeManipulator: public Manipulator {
public:

    CompositeManipulator(const NodePtr &node): Manipulator(node) {}

    bool onMousePressed(QMouseEvent *event) override ;
    bool onMouseReleased(QMouseEvent *event) override ;
    bool onMouseMoved(QMouseEvent *event) override ;

protected:
    void addComponent(const ManipulatorPtr &m) ;

    std::vector<ManipulatorPtr> components_ ;
};

namespace impl {
bool computeRayProjectionOnLine(const Eigen::Vector3f &pA, const Eigen::Vector3f &pB, // line segment
                          const Eigen::Vector3f &o, const Eigen::Vector3f &v, // ray
                          Eigen::Vector3f &p, float &d, float &s );
}

}

#endif
