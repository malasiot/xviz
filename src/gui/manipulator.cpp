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


}
