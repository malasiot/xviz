#ifndef XVIZ_GUI_MANIPULATOR_HPP
#define XVIZ_GUI_MANIPULATOR_HPP

#include <xviz/scene/node.hpp>
#include <xviz/scene/raycaster.hpp>
#include <functional>

class QMouseEvent ;

namespace xviz {

class Manipulator ;
using ManipulatorPtr = std::shared_ptr<Manipulator> ;



class Manipulator {
public:
    Manipulator() {}

    virtual bool onMousePressed(QMouseEvent *event) = 0 ;
    virtual bool onMouseReleased(QMouseEvent *event) = 0;
    virtual bool onMouseMoved(QMouseEvent *event) = 0 ;
};

enum TransformGizmoEvent { TRANSFORM_GIZMO_MOTION_STARTED, TRANSFORM_GIZMO_MOVING, TRANSFORM_GIZMO_MOTION_ENDED } ;

using TransformGizmoCallback = std::function<void (TransformGizmoEvent, const Eigen::Affine3f &tr)> ;

class TransformGizmo: public Manipulator, public Node {
public:
    TransformGizmo(const CameraPtr &cam, float radius) ;

    bool onMousePressed(QMouseEvent *event) override ;
    bool onMouseReleased(QMouseEvent *event) override ;
    bool onMouseMoved(QMouseEvent *event) override ;

    void attachTo(const NodePtr &node) ;
    void setLocalTransform(bool v) ;
    void setCallback(TransformGizmoCallback cb) { cb_ = cb ; }

private:

    enum ComponentId { TX = 0, TY = 1, TZ = 2, TXY = 3, TYZ = 4, TXZ = 5, RX = 6, RY = 7, RZ = 8, N_COMPONENTS  } ;

    struct Component {
        NodePtr node_, picking_  ;
        MaterialPtr mat_ ;
        Eigen::Vector4f clr_ ;

        void setMaterialColor(const Eigen::Vector4f &clr) ;
    };

    int hitTest(QMouseEvent *e, RayCastResult &r) ;
    void createAxisTranslationNode(Component &c, float len, const Eigen::Vector3f &axis, const Eigen::Vector4f &clr) ;
    void createPlaneTranslationNode(Component &c, float sz, const Eigen::Vector3f &axis, const Eigen::Vector4f &clr) ;
    void createRotateAxisNode(Component &c, float rad, const Eigen::Vector3f &axis, const Eigen::Vector4f &clr) ;
    void setTranslation(const Eigen::Vector3f &v) ;
    void setRotation(const Eigen::Matrix3f &m) ;
    void updateTransforms() ;

    void setSelection(int c) ;
     void highlight(int c, bool v);

    Component components_[N_COMPONENTS] ;
    RayCaster ray_caster_ ;
    int selected_ = -1, dragging_ = -1 ;
    Eigen::Vector4f pick_clr_{1, 1, 0, 1} ;
    Eigen::Vector3f start_drag_, start_pos_ ;
    Eigen::Affine3f start_tr_ ;
    Eigen::Vector3f position_ = {0, 0, 0};
    Eigen::Matrix3f orientation_ = Eigen::Matrix3f::Identity(), start_orientation_;
    CameraPtr camera_ ;
    NodePtr transform_node_ ;
    bool local_ = false ;
    TransformGizmoCallback cb_ = nullptr ;

};

namespace impl {
bool computeRayProjectionOnLine(const Eigen::Vector3f &pA, const Eigen::Vector3f &pB, // line segment
                          const Eigen::Vector3f &o, const Eigen::Vector3f &v, // ray
                          Eigen::Vector3f &p, float &d, float &s );
}

}

#endif
