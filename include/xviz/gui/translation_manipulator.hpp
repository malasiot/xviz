#ifndef XVIZ_GUI_TRANSLATION_MANIPULATOR_HPP
#define XVIZ_GUI_TRANSLATION_MANIPULATOR_HPP

#include <xviz/gui/manipulator.hpp>

namespace xviz {

class Translate1DManipulator: public Manipulator {
public:
    Translate1DManipulator(const NodePtr &node, const Eigen::Vector3f &start, const Eigen::Vector3f &end) ;

    void setColor(const Eigen::Vector4f &clr) ;
    void setPickColor(const Eigen::Vector4f &clr) ;

    bool onMousePressed(QMouseEvent *event) override ;
    bool onMouseReleased(QMouseEvent *event) override ;
    bool onMouseMoved(QMouseEvent *event) override ;
    void onCameraUpdated() override ;

    bool hitTest(const Ray &ray, float &t) override ;

protected:
    void setSelected(bool v) override ;
private:
    Eigen::Vector3f start_, end_, start_drag_, end_drag_, translation_init_ ;
    Eigen::Vector4f clr_{1, 0, 0, 1}, pick_clr_{1, 1, 0, 1} ;
    Eigen::Isometry3f linetr_ ;
    Eigen::Affine3f tr_init_ ;
    float len_ ;
    GeometryPtr cone_, line_ ;
    MaterialPtr mat_ ;
    bool dragging_ = false ;
    float pick_threshold_  ;

    void setMaterialColor(const Eigen::Vector4f &clr);
};

class TranslatePlaneManipulator: public Manipulator {
public:
    TranslatePlaneManipulator(const NodePtr &node, float sz, const Eigen::Vector3f &plane) ;

    void setColor(const Eigen::Vector4f &clr) ;
    void setPickColor(const Eigen::Vector4f &clr) ;

    bool onMousePressed(QMouseEvent *event) override ;
    bool onMouseReleased(QMouseEvent *event) override ;
    bool onMouseMoved(QMouseEvent *event) override ;
    void onCameraUpdated() override ;

    bool hitTest(const Ray &ray, float &t) override ;

protected:
    void setSelected(bool v) override ;
private:
    Eigen::Vector3f axis_, start_drag_, translation_init_, v0_, v1_, v2_, v3_ ;
    Eigen::Vector4f clr_{1, 0, 0, 0.5}, pick_clr_{1, 1, 0, 1} ;
    Eigen::Isometry3f planetr_ ;
    Eigen::Affine3f tr_init_ ;
    float sz_ ;
    GeometryPtr cone_, line_ ;
    MaterialPtr mat_ ;
    bool dragging_ = false ;
    float pick_threshold_  ;

    void setMaterialColor(const Eigen::Vector4f &clr);
};

class TranslateXYZManipulator: public CompositeManipulator {
public:
    TranslateXYZManipulator(const NodePtr &node, float line_width) ;

private:
    ManipulatorPtr mx_, my_, mz_, mxy_, myz_, mxz_ ;
};


}

#endif
