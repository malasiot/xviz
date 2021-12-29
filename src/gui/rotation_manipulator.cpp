#include <xviz/gui/rotation_manipulator.hpp>
#include <xviz/scene/detail/intersect.hpp>

#include <xviz/gui/manipulator.hpp>
#include <xviz/scene/node_helpers.hpp>
#include "renderer/util.hpp"
#include <QMouseEvent>
#include <QDebug>

#include <iostream>

using namespace Eigen ;
using namespace std ;

namespace xviz {

// need to handle the case that the ray is parallel to plane

bool intersectPlane(const Vector3f &n, const Vector3f &orig, const Vector3f &dir, Vector3f &p) {

    Vector3f l = dir.normalized() ;

    // assuming vectors are all normalized
    float denom = n.dot(l) ;
    if (fabs(denom) > 0.1) {
        float t = -orig.dot(n)/ denom;
         p = orig + t * l ;
        return true ;
    }

    return false ;
}

bool intersectsSphere(const Ray &r, float rad, Vector3f &p) {
    float t ;
    if ( !detail::rayIntersectsSphere(r, Vector3f{0.f, 0.f, 0.f}, rad, t )) return false ;
    p = r.origin() + t * r.dir() ;
    return true ;
}

RotateAxisManipulator::RotateAxisManipulator(const NodePtr &node, const Eigen::Vector3f &axis, float radius): Manipulator(node), axis_(axis), radius_(radius) {
    mat_.reset(new ConstantMaterial(clr_)) ;
    mat_->setSide(Material::Side::Both);
    mat_->enableDepthTest(false) ;

    GeometryPtr circle_geom(new Geometry(std::move(Geometry::createSolidTorus(radius, 0.025 *radius, 21, 65)))) ;

    ctr_.setIdentity() ;
    ctr_.linear() = rotationBetween({0, 1, 0}, axis_) ;

    NodePtr circle_node(new Node) ;
    circle_node->addDrawable(circle_geom, mat_) ;
    circle_node->transform().linear() = rotationBetween({0, 0, 1}, axis_) ;

    addChild(circle_node) ;
}

void RotateAxisManipulator::setColor(const Eigen::Vector4f &clr)
{
    clr_ = clr ;
    setMaterialColor(clr);

}

void RotateAxisManipulator::setPickColor(const Vector4f &clr)
{
    pick_clr_ = clr ;
}

void RotateAxisManipulator::setMaterialColor(const Eigen::Vector4f &clr) {
    ConstantMaterial *cm = static_cast<ConstantMaterial *>(mat_.get()) ;
    cm->setColor(clr) ;
}


bool RotateAxisManipulator::onMousePressed(QMouseEvent *event) {
    Ray ray = camera_->getRay(event->x(), event->y()) ;
    Affine3f tf = transform_node_->globalTransform().inverse() ;

    Ray tr(ray, ctr_ * tf) ;

    float t ;

    if ( detail::rayIntersectsTorus(tr, radius_, 0.05 * radius_, t)) {
        Vector3f p = tr.origin() + t * tr.dir() ;
        start_drag_ = ctr_.inverse() * p ;
        dragging_ = true ;
        rotation_init_ = transform_node_->transform().linear() ;
        tr_init_ = tf ;
        setMaterialColor(pick_clr_) ;
        prev_tr_ = transform_node_->transform() ;
        if ( callback_)
            callback_(ManipulatorEvent::MOTION_STARTED, transform_node_->transform()) ;
        return true ;
    }

    return false ;
}

bool RotateAxisManipulator::onMouseReleased(QMouseEvent *event) {
    if ( dragging_ ) {
        dragging_ = false ;
        setMaterialColor(clr_) ;
        if ( callback_)
            callback_(ManipulatorEvent::MOTION_ENDED, transform_node_->transform()) ;
        return true ;
    }

    return false ;
}

bool RotateAxisManipulator::onMouseMoved(QMouseEvent *event)
{
    Ray ray = camera_->getRay(event->x(), event->y()) ;
    Affine3f tf = transform_node_->globalTransform().inverse() ;

    if ( dragging_ ) {
        Ray tr(ray, tr_init_) ; // ray transform to local coordinate system

        Vector3f p ;

        if ( intersectPlane(axis_, tr.origin(), tr.dir(), p) || intersectsSphere(tr, radius_, p)) {

            float angle = acos(p.normalized().dot(start_drag_.normalized())) ;
            Vector3f vc = p.cross(start_drag_) ;
            if ( vc.dot(axis_) > 0 )
                angle = -angle ;
            //     qDebug() << p.x() << p.y() << p.z() << angle * 180 /M_PI ;
            transform_node_->transform().linear() = rotation_init_ * AngleAxisf(angle, axis_) ;
            if ( callback_)
                callback_(ManipulatorEvent::MOVING, transform_node_->transform()) ;
            return true;
        }

    } else {
        Ray tr(ray, ctr_ * tf) ;

        float t ;
        if ( detail::rayIntersectsTorus(tr, radius_, 0.05 * radius_, t)) {
            setMaterialColor(pick_clr_) ;
            return true ;
        } else {
             setMaterialColor(clr_) ;
        }

    }


    return false ;
}

void RotateAxisManipulator::onCameraUpdated()
{
    /*
    Vector4f cc = camera_->getViewMatrix().inverse() * Vector4f(0, 0, -1, 1)  ;
    Affine3f tf = transform_node_->globalTransform().inverse() ;
    float dist = (cc - tf * Vector4f(0, 0, 0, 1)).norm() ;

    const float pixel_scale = 100.f ;
   const PerspectiveCamera *pc = dynamic_cast<PerspectiveCamera *>(camera_.get()) ;
    float scale = 2 * (tan(pc->yFov()/2) * dist) * (pixel_scale / camera_->getViewport().height_) ;

    cout << dist << ' ' << scale << ' ' << pc->yFov() << endl ;


   scale_node_->transform().linear() = Matrix3f::Identity() *scale / radius_;

    // cout << camera_->getViewMatrix().inverse() * Vector4f(0, 0, -1, 0) ;
     //float dist = length(position - g.active_state.cam.position);
     //   return std::tan(g.active_state.cam.yfov) * dist * (pixel_scale / g.active_state.viewport_size.y);
*/

}

RotateXYZManipulator::RotateXYZManipulator(const Node::NodePtr &node, float radius): CompositeManipulator(node)
{
    RotateAxisManipulator *mx = new RotateAxisManipulator(node, {1, 0, 0}, radius) ;
    mx->setColor({1, 0, 0, 1}) ;
    mx_.reset(mx) ;

    RotateAxisManipulator *my = new RotateAxisManipulator(node, {0, 1, 0}, radius) ;
    my->setColor({0, 1, 0, 1}) ;
    my_.reset(my) ;

    RotateAxisManipulator *mz = new RotateAxisManipulator(node, {0, 0, 1}, radius) ;
    mz->setColor({0, 0, 1, 1}) ;
    mz_.reset(mz) ;

    addComponent(mx_);
    addComponent(my_) ;
    addComponent(mz_) ;
}

}
