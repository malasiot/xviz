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
    virtual bool hitTest(const Ray &ray, float &t) { return false ; }

    const Eigen::Affine3f &lastTransform() const { return prev_tr_ ; }

    bool isSelected() const { return selected_ ; }
    bool isDragging() const { return dragging_ ; }

protected:
    friend class CompositeManipulator ;

    virtual void setSelected(bool v) { selected_ = v ; }

    CameraPtr camera_ ;
    NodePtr transform_node_ ;
    Eigen::Affine3f prev_tr_ ;
    Callback callback_ = nullptr ;
    bool selected_ = false ;
    bool dragging_ = false ;
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
    ManipulatorPtr selected_, current_ ;
};

class TransformGizmo: public Manipulator {
public:
    TransformGizmo(const CameraPtr &cam, float radius, const NodePtr &node) ;

    bool onMousePressed(QMouseEvent *event) override ;
    bool onMouseReleased(QMouseEvent *event) override ;
    bool onMouseMoved(QMouseEvent *event) override ;
    void onCameraUpdated() override ;

private:

    enum ComponentId { TX = 0, TY = 1, TZ = 2, TXY = 3, TYZ = 4, TXZ = 5, RX = 7, RY = 8, RZ = 9, N_COMPONENTS  } ;

    struct Component {
        NodePtr node_ ;
        MaterialPtr mat_ ;
        Eigen::Vector4f clr_ ;

        void setMaterialColor(const Eigen::Vector4f &clr) ;
    };

    int hitTest(QMouseEvent *e, RayCastResult &r) ;
    void createAxisTranslationNode(Component &c, float len, const Eigen::Vector3f &axis, const Eigen::Vector4f &clr) ;
    void createPlaneTranslationNode(Component &c, float sz, const Eigen::Vector3f &axis, const Eigen::Vector4f &clr) ;
    void createRotateAxisNode(Component &c, float rad, const Eigen::Vector3f &axis, const Eigen::Vector4f &clr) ;
    void translateAxis(const Eigen::Vector3f &axis, const Eigen::Vector3f &start_pt, const Ray &ray) ;

    void setSelection(int c) ;
     void highlight(int c, bool v);

    Component components_[N_COMPONENTS] ;
    RayCaster ray_caster_ ;
    int selected_ = -1, dragging_ = -1 ;
    Eigen::Vector4f pick_clr_{1, 1, 0, 1} ;
    Eigen::Vector3f start_drag_ ;
    Eigen::Isometry3f start_tr_ ;

};

namespace impl {
bool computeRayProjectionOnLine(const Eigen::Vector3f &pA, const Eigen::Vector3f &pB, // line segment
                          const Eigen::Vector3f &o, const Eigen::Vector3f &v, // ray
                          Eigen::Vector3f &p, float &d, float &s );
}

}

#endif
