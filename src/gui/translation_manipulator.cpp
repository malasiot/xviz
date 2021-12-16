#include <xviz/gui/translation_manipulator.hpp>

#include <xviz/gui/manipulator.hpp>
#include <xviz/scene/node_helpers.hpp>
#include "renderer/util.hpp"
#include <QMouseEvent>
#include <QDebug>

using namespace Eigen ;
using namespace std ;

namespace xviz {

// find closest point


Translate1DManipulator::Translate1DManipulator(const NodePtr &node, const Eigen::Vector3f &start, const Eigen::Vector3f &end): Manipulator(node), start_(start), end_(end) {
    mat_.reset(new ConstantMaterial(clr_)) ;
    mat_->enableDepthTest(false) ;

    GeometryPtr line_geom(new Geometry(Geometry::Lines)) ;
    line_geom->vertices().push_back(start) ;
    line_geom->vertices().push_back(end) ;

    Vector3f dir = (end_ - start_) ;
    float len = dir.norm() ;
    dir /= len ;

    pick_threshold_ = 0.075 * len ;

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
    Ray tr(ray, tf) ; // ray transform to local coordinate system

    float d, s, len = (end_ - start_).norm() ;

    if ( impl::computeRayProjectionOnLine(start_, end_, tr.origin(), tr.dir(), start_drag_, d, s ) && d < pick_threshold_ && s >= 0 && s <= len ) {
        dragging_ = true ;
        translation_init_ = transform_node_->transform().translation() ;
        setMaterialColor(pick_clr_) ;
        return true ;
    }

//    qDebug() << d << start_drag_.x() << start_drag_.y() << start_drag_.z() ;

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
        float d, s ;
        if ( impl::computeRayProjectionOnLine(start_, end_, tr.origin(), tr.dir(), p, d, s) ) {
            Vector3f t = translation_init_ + p - start_drag_  ;
            if ( transform_node_ ) transform_node_->transform().translation() = t  ;

            return true ;
        }

    }

    return false ;
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
