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
            setSelected(true) ;
            cout << name() << ' ' << t << endl ;
            return true ;
        }
        else {
            setMaterialColor(clr_) ;
            setSelected(false) ;
            return false ;
        }

    }

    return false ;
}

void Translate1DManipulator::setSelected(bool v)
{
    selected_ = v ;
    if ( v )
        setMaterialColor(pick_clr_) ;
    else
        setMaterialColor(clr_) ;
}

void Translate1DManipulator::onCameraUpdated()
{

}

bool Translate1DManipulator::hitTest(const Ray &ray, float &t)
{
     Affine3f tf = parent()->globalTransform().inverse() ;
     return detail::rayIntersectsCylinder(Ray(ray, linetr_ * tf), len_ * 0.05f, len_ + 0.1f * len_, t ) ;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


TranslatePlaneManipulator::TranslatePlaneManipulator(const NodePtr &node, float sz, const Eigen::Vector3f &nrm):
    Manipulator(node), sz_(sz), axis_(nrm) {
    mat_.reset(new ConstantMaterial(clr_)) ;
    mat_->setSide(Material::Side::Both) ;
    mat_->enableDepthTest(false) ;

    GeometryPtr plane_geom(new Geometry(Geometry::makePlane(sz, sz, 2, 2))) ;

    planetr_.setIdentity() ;
    planetr_.linear() = rotationBetween({0, 1, 0}, nrm) ;
    planetr_.translation() = planetr_.linear().inverse() * Vector3f{sz, 0, sz} ;

    v0_ = planetr_ * Vector3f{-sz/2, 0, -sz/2} ;
    v1_ = planetr_ * Vector3f{sz/2, 0, -sz/2} ;
    v2_ = planetr_ * Vector3f{sz/2, 0, sz/2} ;
    v3_ = planetr_ * Vector3f{-sz/2, 0, sz/2} ;

    NodePtr plane_node(new Node) ;
    plane_node->addDrawable(plane_geom, mat_) ;
    plane_node->setTransform(planetr_) ;

    addChild(plane_node) ;

}



void TranslatePlaneManipulator::setColor(const Eigen::Vector4f &clr)
{
    clr_ = clr ;
    setMaterialColor(clr);

}

void TranslatePlaneManipulator::setPickColor(const Vector4f &clr)
{
    pick_clr_ = clr ;
}

void TranslatePlaneManipulator::setMaterialColor(const Eigen::Vector4f &clr) {
    ConstantMaterial *cm = static_cast<ConstantMaterial *>(mat_.get()) ;
    cm->setColor(clr) ;
}
bool TranslatePlaneManipulator::onMousePressed(QMouseEvent *event)
{
    Ray ray = camera_->getRay(event->x(), event->y()) ;
    Affine3f tf = parent()->globalTransform().inverse() ;
    Ray tr(ray,  tf) ; // ray transform to local coordinate system

    float t ;
    if (  detail::rayIntersectsTriangle(tr, v0_, v1_, v2_, false, t) ||
          detail::rayIntersectsTriangle(tr, v0_, v2_, v3_, false, t) ) {
        Vector3f p = tr.origin() + t * tr.dir() ;
        start_drag_ =   p ;
        translation_init_ = transform_node_->transform().translation() ;
        dragging_ = true ;
        tr_init_ = tf ;

        if ( callback_)
            callback_(ManipulatorEvent::MOTION_STARTED, transform_node_->transform()) ;
        return true ;
    }


    return false ;
}

bool TranslatePlaneManipulator::onMouseReleased(QMouseEvent *event) {
    if ( dragging_ ) {
        dragging_ = false ;
        setMaterialColor(clr_) ;
        if ( callback_)
            callback_(ManipulatorEvent::MOTION_ENDED, transform_node_->transform()) ;
        return true ;
    }

    return false ;
}

extern bool intersectPlane(const Vector3f &n, const Vector3f &orig, const Vector3f &dir, Vector3f &p);

bool TranslatePlaneManipulator::onMouseMoved(QMouseEvent *event)
{

    Ray ray = camera_->getRay(event->x(), event->y()) ;
    Affine3f tf = parent()->globalTransform().inverse() ;
    Ray tr(ray, tf) ; // ray transform to local coordinate system

    if ( dragging_ ) {
        Ray tr(ray, tr_init_) ; // ray transform to local coordinate system
        Vector3f p ;

        if ( intersectPlane(axis_, tr.origin(), tr.dir(), p) ) {
            Vector3f t = translation_init_ + p - start_drag_  ;

            if ( transform_node_ ) transform_node_->transform().translation() = t  ;
            if ( callback_)
                callback_(ManipulatorEvent::MOVING, transform_node_->transform()) ;

            return true ;
        }

    } else {
        float t ;
        if (  detail::rayIntersectsTriangle(tr, v0_, v1_, v2_, false, t) ||
              detail::rayIntersectsTriangle(tr, v0_, v2_, v3_, false, t) ) {

            setSelected(true) ;
        } else
            setSelected(false) ;
    }

    return false ;
}

void TranslatePlaneManipulator::setSelected(bool v)
{
    selected_ = v ;
    if ( v )
        setMaterialColor(pick_clr_) ;
    else
        setMaterialColor(clr_) ;
}


void TranslatePlaneManipulator::onCameraUpdated()
{

}

bool TranslatePlaneManipulator::hitTest(const Ray &ray, float &t) {
    Affine3f tf = parent()->globalTransform().inverse() ;
    Ray tr(ray, tf) ;
    return (  detail::rayIntersectsTriangle(tr, v0_, v1_, v2_, false, t) ||
          detail::rayIntersectsTriangle(tr, v0_, v2_, v3_, false, t) ) ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
TranslateXYZManipulator::TranslateXYZManipulator(const NodePtr &n, float hw): CompositeManipulator(n) {
    Translate1DManipulator *mx = new Translate1DManipulator(n, {-hw, 0, 0},{hw, 0, 0}) ;
    mx->setColor({1, 0, 0, 1}) ;
    mx_.reset(mx) ;
    mx_->setName("X") ;

    Translate1DManipulator *my = new Translate1DManipulator(n, {0, -hw, 0},{0, hw, 0}) ;
    my->setColor({0, 1, 0, 1}) ;
    my_.reset(my) ;
    my_->setName("Y") ;

    Translate1DManipulator *mz = new Translate1DManipulator(n, {0, 0, -hw},{0, 0, hw}) ;
    mz->setColor({0, 0, 1, 1}) ;
    mz_.reset(mz) ;
    mz_->setName("Z") ;

    TranslatePlaneManipulator *myz = new TranslatePlaneManipulator(n, hw/4, {1, 0, 0}) ;
    myz->setColor({1, 0, 0, 0.5}) ;
    myz_.reset(myz) ;
    myz_->setName("YZ") ;

    TranslatePlaneManipulator *mxz = new TranslatePlaneManipulator(n, hw/4, {0, 1, 0}) ;
    mxz->setColor({0, 1, 0, 0.5}) ;
    mxz_.reset(mxz) ;
    mxz_->setName("XZ") ;

    TranslatePlaneManipulator *mxy = new TranslatePlaneManipulator(n, hw/4, {0, 0, 1}) ;
    mxy->setColor({0, 0, 1, 0.5}) ;
    mxy_.reset(mxy) ;
    mxy_->setName("XY") ;

    addComponent(myz_);
    addComponent(mxz_) ;
    addComponent(mxy_) ;

    addComponent(mx_);
    addComponent(my_) ;
    addComponent(mz_) ;
}




}
