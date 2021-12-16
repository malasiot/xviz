#include <xviz/gui/rotation_manipulator.hpp>


#include <xviz/gui/manipulator.hpp>
#include <xviz/scene/node_helpers.hpp>
#include "renderer/util.hpp"
#include <QMouseEvent>
#include <QDebug>

using namespace Eigen ;
using namespace std ;

namespace xviz {

// need to handle the case that the ray is parallel to plabe

bool intersectPlane(const Vector3f &n, const Vector3f &orig, const Vector3f &dir, Vector3f &p) {

    Vector3f l = dir.normalized() ;

    // assuming vectors are all normalized
     float denom = n.dot(l) ;
     if (fabs(denom) > 1e-6) {
           float t = -orig.dot(n)/ denom;
         //  qDebug() << t ;
           if ( t > 0 ) {
               p = orig + t * l ;
               return true ;
           }
     }

     return false ;
}

RotateAxisManipulator::RotateAxisManipulator(const NodePtr &node, const Eigen::Vector3f &axis, float radius): Manipulator(node), axis_(axis), radius_(radius) {
    mat_.reset(new ConstantMaterial(clr_)) ;
    mat_->setSide(Material::Side::Both);
    mat_->enableDepthTest(false) ;

    GeometryPtr circle_geom(new Geometry(std::move(Geometry::createSolidCylinder(radius, 0.05 *radius, 36, 2, false)))) ;

    pick_threshold_ = 0.075 * radius_ ;

    NodePtr circle_node(new Node) ;
    circle_node->addDrawable(circle_geom, mat_) ;
    circle_node->transform().linear() = rotationBetween({0, 0, 1}, axis) ;

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

bool RotateAxisManipulator::onMousePressed(QMouseEvent *event)
{
    Ray ray = camera_->getRay(event->x(), event->y()) ;
    Affine3f tf = transform_node_->globalTransform().inverse() ;
    Ray tr(ray, tf) ; // ray transform to local coordinate system

    if ( intersectPlane(axis_, tr.origin(), tr.dir(), start_drag_) && fabs(start_drag_.norm() - radius_) < pick_threshold_ ) {
        dragging_ = true ;
        rotation_init_ = transform_node_->transform().linear() ;
        tr_init_ = tf ;
        setMaterialColor(pick_clr_) ;
    }

    return false ;
}

bool RotateAxisManipulator::onMouseReleased(QMouseEvent *event) {
    if ( dragging_ ) {
        dragging_ = false ;
        setMaterialColor(clr_) ;
        return true ;
    }

    return false ;
}

bool RotateAxisManipulator::onMouseMoved(QMouseEvent *event)
{
    Ray ray = camera_->getRay(event->x(), event->y()) ;
 //   Affine3f tf = transform_node_->globalTransform().inverse() ;
    Ray tr(ray, tr_init_) ; // ray transform to local coordinate system


    if ( dragging_ ) {
        Vector3f p ;

        if ( intersectPlane(axis_, tr.origin(), tr.dir(), p) ) {

            float angle = acos(p.normalized().dot(start_drag_.normalized())) ;
            Vector3f vc = p.cross(start_drag_) ;
            if ( vc.dot(axis_) > 0 )
                angle = -angle ;
            qDebug() << p.x() << p.y() << p.z() << angle * 180 /M_PI ;
             transform_node_->transform().linear() = rotation_init_ * AngleAxisf(angle, axis_) ;
             return true;
        }

    }


    return false ;
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
