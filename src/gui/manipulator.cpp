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


bool CompositeManipulator::onMousePressed(QMouseEvent *event) {
    for( const auto &m: components_ ) {
        if ( m->onMousePressed(event) ) return true ;
    }
    return false ;
}

bool CompositeManipulator::onMouseReleased(QMouseEvent *event) {
    for( const auto &m: components_ ) {
        if ( m->onMouseReleased(event) ) return true ;
    }
    return false ;
}

bool CompositeManipulator::onMouseMoved(QMouseEvent *event) {
    for( const auto &m: components_ ) {
        if ( m->onMouseMoved(event) ) return true ;
    }
    return false ;
}

void CompositeManipulator::addComponent(const ManipulatorPtr &m)
{
    components_.push_back(m) ;
    addChild(m);
}

namespace impl {

bool computeRayProjectionOnLine(const Eigen::Vector3f &pA, const Eigen::Vector3f &pB, // line segment
                          const Eigen::Vector3f &o, const Eigen::Vector3f &v, // ray
                          Eigen::Vector3f &p, float &d, float &s )
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

    float denom1 = d1.dot(n2) ;
    float denom2 = d2.dot(n1) ;

    if ( fabs(denom1) < std::numeric_limits<float>::min() ) return false ;

    s = (p2 - p1).dot(n2) / denom1 ;
    float t = (p1 - p2).dot(n1) / denom2 ;

//    qDebug() << 'D' << denom1 << s ;
    p = p1 + s * d1 ;
    auto q = p2 + t * d2 ;

    d = ( q - p ).norm();

    return true ;
}
}


}
