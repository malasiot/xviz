#include <xviz/gui/scaling_manipulator.hpp>


#include <xviz/gui/manipulator.hpp>
#include <xviz/scene/node_helpers.hpp>
#include "renderer/util.hpp"
#include <QMouseEvent>
#include <QDebug>

using namespace Eigen ;
using namespace std ;

namespace xviz {

Scale1DManipulator::Scale1DManipulator(const NodePtr &node, const Eigen::Vector3f &start, const Eigen::Vector3f &end): Manipulator(node), start_(start), end_(end) {
    mat_.reset(new ConstantMaterial(clr_)) ;

    GeometryPtr line_geom(new Geometry(Geometry::Lines)) ;
    line_geom->vertices().push_back(start) ;
    line_geom->vertices().push_back(end) ;

    Vector3f dir = (end_ - start_) ;
    float len = dir.norm() ;
    dir /= len ;

    pick_threshold_ = 0.075 * len ;

    float hs = len * 0.025f ;
    GeometryPtr box_geom(new Geometry(Geometry::createSolidCube({hs, hs, hs}))) ;

    NodePtr line_node(new Node) ;
    line_node->addDrawable(line_geom, mat_) ;

    NodePtr left_box(new Node) ;
    left_box->addDrawable(box_geom, mat_) ;
    left_box->transform().translation() = start_ ;

    NodePtr right_box(new Node) ;
    right_box->addDrawable(box_geom, mat_) ;
    right_box->transform().translation() = end_ ;

    addChild(line_node) ;
    addChild(left_box) ;
    addChild(right_box) ;
}

void Scale1DManipulator::setColor(const Eigen::Vector4f &clr)
{
    clr_ = clr ;
    setMaterialColor(clr);

}

void Scale1DManipulator::setPickColor(const Vector4f &clr)
{
    pick_clr_ = clr ;
}

void Scale1DManipulator::setMaterialColor(const Eigen::Vector4f &clr) {
    ConstantMaterial *cm = static_cast<ConstantMaterial *>(mat_.get()) ;
    cm->setColor(clr) ;
}

bool Scale1DManipulator::onMousePressed(QMouseEvent *event)
{
    Ray ray = camera_->getRay(event->x(), event->y()) ;
    Affine3f tf = parent()->globalTransform().inverse() ;
    Ray tr(ray, tf) ; // ray transform to local coordinate system

    float d, s, len = (end_ - start_).norm() ;

    if ( impl::computeRayProjectionOnLine(start_, end_, tr.origin(), tr.dir(), start_drag_, d, s ) && d < pick_threshold_ && s >= 0 && s <= len ) {
        dragging_ = true ;
        start_s_ = s ;
        transform_node_->transform().computeRotationScaling(&rotation_init_, &scaling_init_) ;
        setMaterialColor(pick_clr_) ;
        return true ;
    }

//    qDebug() << d << start_drag_.x() << start_drag_.y() << start_drag_.z() ;

    return false ;
}

bool Scale1DManipulator::onMouseReleased(QMouseEvent *event) {
    if ( dragging_ ) {
        dragging_ = false ;
        setMaterialColor(clr_) ;
        return true ;
    }

    return false ;
}

bool Scale1DManipulator::onMouseMoved(QMouseEvent *event)
{
    if ( dragging_ ) {
        Ray ray = camera_->getRay(event->x(), event->y()) ;
        Affine3f tf = parent()->globalTransform().inverse() ;
        Ray tr(ray, tf) ; // ray transform to local coordinate system

        float len = (end_ - start_).norm();
        Vector3f dir = (end_ - start_)/len ;
        int idx ;
        dir.maxCoeff(&idx) ;

        Vector3f p ;
        float d, s ;
        if ( impl::computeRayProjectionOnLine(start_, end_, tr.origin(), tr.dir(), p, d, s) ) {
            float scale = (2*(s - start_s_) + len ) / len ;

            Vector3f t{1, 1, 1} ;
            t[idx] = scale ;
            qDebug() << t.x() << t.y() << t.z() ;
            Matrix3f m = rotation_init_ * scaling_init_ * t.asDiagonal() ;
            if ( transform_node_ ) {
                transform_node_->transform().linear() = m  ;
            }

            return true ;
        }

    }

    return false ;
}

}
