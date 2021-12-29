#include <xviz/gui/translation_manipulator.hpp>

#include <xviz/gui/manipulator.hpp>
#include <xviz/scene/node_helpers.hpp>
#include <xviz/scene/detail/intersect.hpp>
#include "renderer/util.hpp"
#include <QMouseEvent>
#include <QDebug>

#include <iostream>

using namespace Eigen ;
using namespace std ;

namespace xviz {

// find closest point


Translate1DManipulator::Translate1DManipulator(const NodePtr &node, const Eigen::Vector3f &start, const Eigen::Vector3f &end): Manipulator(node), start_(start), end_(end) {
    mat_.reset(new ConstantMaterial(clr_)) ;
    mat_->enableDepthTest(false) ;

    Vector3f dir = (end_ - start_) ;
    len_ = dir.norm() ;
    dir /= len_ ;

    pick_threshold_ = 3 ;

    GeometryPtr cone_geom(new Geometry(Geometry::createSolidCone(len_ * 0.025f, len_ * 0.1f, 10, 10))) ;

    GeometryPtr cyl_geom(new Geometry(Geometry::createSolidCylinder(len_ * 0.01f, len_, 10, 10)));

    linetr_.setIdentity() ;
    linetr_.linear() = rotationBetween({0, 0, 1}, Vector3f(dir)) ;
    linetr_.translation() = (start_ + end_)/2 ;

    NodePtr line_node(new Node) ;
    line_node->addDrawable(cyl_geom, mat_) ;
    line_node->setTransform(linetr_) ;

    NodePtr left_cone(new Node) ;
    left_cone->addDrawable(cone_geom, mat_) ;
    left_cone->transform().linear() = rotationBetween({0, 0, 1}, Vector3f(-dir)) ;
    left_cone->transform().translation() = start_ ;

    NodePtr right_cone(new Node) ;
    right_cone->addDrawable(cone_geom, mat_) ;
    right_cone->transform().linear() = rotationBetween({0, 0, 1}, dir) ;
    right_cone->transform().translation() = end_ ;

    addChild(line_node) ;
    addChild(left_cone) ;
    addChild(right_cone) ;


}

void Translate1DManipulator::setColor(const Eigen::Vector4f &clr)
{
    clr_ = clr ;
    setMaterialColor(clr);

}

void Translate1DManipulator::setPickColor(const Vector4f &clr)
{
    pick_clr_ = clr ;
}

void Translate1DManipulator::setMaterialColor(const Eigen::Vector4f &clr) {
    ConstantMaterial *cm = static_cast<ConstantMaterial *>(mat_.get()) ;
    cm->setColor(clr) ;
}
bool Translate1DManipulator::onMousePressed(QMouseEvent *event)
{
    Ray ray = camera_->getRay(event->x(), event->y()) ;
    Affine3f tf = parent()->globalTransform().inverse() ;
    Ray tr(ray, linetr_ * tf) ; // ray transform to local coordinate system

    float t ;

    if ( detail::rayIntersectsCylinder(tr, len_ * 0.05f, len_ + 0.1f * len_, t )) {
        Vector3f p = tr.origin() + t * tr.dir() ;
        start_drag_ = linetr_.inverse() * p ;
        dragging_ = true ;
        translation_init_ = transform_node_->transform().translation() ;
        tr_init_ = tf ;
        setMaterialColor(pick_clr_) ;
        prev_tr_ = transform_node_->transform() ;
        if ( callback_)
            callback_(ManipulatorEvent::MOTION_STARTED, transform_node_->transform()) ;
        return true ;

    }

    return false ;
}

bool Translate1DManipulator::onMouseReleased(QMouseEvent *event) {
    if ( dragging_ ) {
        dragging_ = false ;
        setMaterialColor(clr_) ;
        if ( callback_)
            callback_(ManipulatorEvent::MOTION_ENDED, transform_node_->transform()) ;
        return true ;
    }

    return false ;
}

bool Translate1DManipulator::onMouseMoved(QMouseEvent *event)
{
    Ray ray = camera_->getRay(event->x(), event->y()) ;
    Affine3f tf = parent()->globalTransform().inverse() ;

    if ( dragging_ ) {
        Ray tr(ray, tr_init_) ; // ray transform to local coordinate system
        Vector3f p ;
        float d, s ;
        if ( impl::computeRayProjectionOnLine(start_, end_, tr.origin(), tr.dir(), p, d, s) ) {

            Vector3f t = translation_init_ + p - start_drag_  ;

            if ( transform_node_ ) transform_node_->transform().translation() = t  ;
            if ( callback_)
                callback_(ManipulatorEvent::MOVING, transform_node_->transform()) ;

            return true ;
        }

    } else {

        float t ;
        if ( detail::rayIntersectsCylinder(Ray(ray, linetr_ * tf), len_ * 0.05f, len_ + 0.1f * len_, t )) {
            setMaterialColor(pick_clr_) ;
            return true ;
        }
        else {
             setMaterialColor(clr_) ;
             return false ;
        }

    }

    return false ;
}

void Translate1DManipulator::onCameraUpdated()
{

}


TranslateXYZManipulator::TranslateXYZManipulator(const NodePtr &n, float hw): CompositeManipulator(n) {
    Translate1DManipulator *mx = new Translate1DManipulator(n, {-hw, 0, 0},{hw, 0, 0}) ;
    mx->setColor({1, 0, 0, 1}) ;
    mx_.reset(mx) ;

    Translate1DManipulator *my = new Translate1DManipulator(n, {0, -hw, 0},{0, hw, 0}) ;
    my->setColor({0, 1, 0, 1}) ;
    my_.reset(my) ;

    Translate1DManipulator *mz = new Translate1DManipulator(n, {0, 0, -hw},{0, 0, hw}) ;
    mz->setColor({0, 0, 1, 1}) ;
    mz_.reset(mz) ;

    addComponent(mx_);
    addComponent(my_) ;
    addComponent(mz_) ;
}


}
