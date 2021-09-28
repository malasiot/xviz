#include <xviz/gui/manipulator.hpp>
#include <xviz/scene/node_helpers.hpp>
#include "renderer/util.hpp"
#include <QMouseEvent>
#include <QDebug>

using namespace Eigen ;
using namespace std ;

namespace xviz {

void Manipulator::setCamera(const CameraPtr &cam) {
    camera_ = cam ;
}

// find closest point

bool computeRayProjectionOnLine(const Eigen::Vector3f &pA, const Eigen::Vector3f &pB, // line segment
                          const Eigen::Vector3f &o, const Eigen::Vector3f &v, // ray
                          Eigen::Vector3f &p, float &d )
{

    Vector3f ab  = pB - pA ;
    float len = ab.norm() ;
    Vector3f d1 = ab / len ;

    const auto &p1 =  pA ;
    const auto &p2 = o ;
    const auto d2 = v.normalized() ;

    auto n = d1.cross(d2) ;
    auto n2 = d2.cross(n) ;
    auto n1 = d1.cross(n) ;

    float denom = d1.dot(n2) ;

    if ( denom < std::numeric_limits<float>::min() ) return false ;

    float s = (p2 - p1).dot(n2) / denom ;
    float t = (p1 - p2).dot(n1) / d2.dot(n1) ;

    if ( s < 0 || s > len ) return false ;

    qDebug() << s ;
    p = p1 + s * d1 ;
    auto q = p2 + t * d2 ;

    d = ( q - p ).norm();

    return true ;
}

Translate1DManipulator::Translate1DManipulator(const Eigen::Vector3f &start, const Eigen::Vector3f &end): Manipulator(), start_(start), end_(end) {
    mat_.reset(new ConstantMaterial(clr_)) ;

    GeometryPtr line_geom(new Geometry(Geometry::Lines)) ;
    line_geom->vertices().push_back(start) ;
    line_geom->vertices().push_back(end) ;

    Vector3f dir = (end_ - start_) ;
    float len = dir.norm() ;
    dir /= len ;

    pick_threshold_ = 0.05 * len ;

    GeometryPtr cone_geom(new Geometry(Geometry::createSolidCone(len * 0.025f, len * 0.1f, 10, 10))) ;

    NodePtr line_node(new Node) ;
    line_node->addDrawable(line_geom, mat_) ;

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

}

void Translate1DManipulator::setPickColor(const Vector4f &clr)
{
    pick_clr_ = clr ;
}

void Translate1DManipulator::setPickThreshold(float t)
{
    pick_threshold_ = t ;
}

void Translate1DManipulator::setMaterialColor(const Eigen::Vector4f &clr) {
    ConstantMaterial *cm = static_cast<ConstantMaterial *>(mat_.get()) ;
    cm->setColor(clr) ;
}
bool Translate1DManipulator::onMousePressed(QMouseEvent *event)
{
    Ray ray = camera_->getRay(event->x(), event->y()) ;
    Affine3f tf = parent()->globalTransform().inverse() ;
    Ray tr(ray, tf) ; // ray transform to local coordinate system

    float d ;

    if ( computeRayProjectionOnLine(start_, end_, tr.origin(), tr.dir(), start_drag_, d ) && d < pick_threshold_ ) {
        dragging_ = true ;
        translation_init_ = transform().translation() ;
        setMaterialColor(pick_clr_) ;
        return true ;
    }

    return false ;
}

bool Translate1DManipulator::onMouseReleased(QMouseEvent *event) {
    if ( dragging_ ) {
        dragging_ = false ;
        setMaterialColor(clr_) ;
        return true ;
    }

    return false ;
}

bool Translate1DManipulator::onMouseMoved(QMouseEvent *event)
{
    if ( dragging_ ) {
        Ray ray = camera_->getRay(event->x(), event->y()) ;
        Affine3f tf = parent()->globalTransform().inverse() ;
        Ray tr(ray, tf) ; // ray transform to local coordinate system

        Vector3f p ;
        float d ;
        if ( computeRayProjectionOnLine(start_, end_, tr.origin(), tr.dir(), p, d) ) {
            Vector3f t = translation_init_ + p - start_drag_  ;
            if ( container_ ) container_->transform().translation() = t  ;
            else transform().translation() = t  ;
        }

        return true ;

    }

    return false ;

}

void CompositeManipulator::addComponent(const ManipulatorPtr &m)
{
  components_.push_back(m) ;
  m->container_ = this ;
}


}
