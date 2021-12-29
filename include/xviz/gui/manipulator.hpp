#ifndef XVIZ_GUI_MANIPULATOR_HPP
#define XVIZ_GUI_MANIPULATOR_HPP

#include <xviz/scene/node.hpp>
#include <xviz/scene/raycaster.hpp>
#include <functional>

class QMouseEvent ;

namespace xviz {

class Manipulator ;
using ManipulatorPtr = std::shared_ptr<Manipulator> ;

enum class ManipulatorEvent { MOTION_STARTED, MOVING, MOTION_ENDED } ;

class Manipulator: public Node {
public:

    using Callback = std::function<void(ManipulatorEvent e, const Eigen::Affine3f &tr)> ;

    Manipulator(const NodePtr &node): transform_node_(node), prev_tr_(node->transform()) {}

    virtual void setCamera(const CameraPtr &cam) ;

    virtual bool onMousePressed(QMouseEvent *event) = 0 ;
    virtual bool onMouseReleased(QMouseEvent *event) = 0;
    virtual bool onMouseMoved(QMouseEvent *event) = 0 ;
    virtual void onCameraUpdated() = 0 ;

    virtual void setCallback(Callback cb) { callback_ = cb ; }

    const Eigen::Affine3f &lastTransform() const { return prev_tr_ ; }

protected:
    friend class CompositeManipulator ;

    CameraPtr camera_ ;
    NodePtr transform_node_ ;
    Eigen::Affine3f prev_tr_ ;
    Callback callback_ = nullptr ;
};

class CompositeManipulator: public Manipulator {
public:

    CompositeManipulator(const NodePtr &node): Manipulator(node) {}

    bool onMousePressed(QMouseEvent *event) override ;
    bool onMouseReleased(QMouseEvent *event) override ;
    bool onMouseMoved(QMouseEvent *event) override ;
    void onCameraUpdated() override ;

    void setCamera(const CameraPtr &cam) override ;
    void setCallback(Callback cb) override ;

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
