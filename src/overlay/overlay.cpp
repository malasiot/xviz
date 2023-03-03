#include <xviz/overlay/overlay.hpp>

#include "common/gl/gl3w.h"

using namespace Eigen ;

namespace xviz {

void OverlayItem::setPosition(float x, float y) {
    x_ = x ; y_ = y ;
}


void OverlayContainer::setSize(const Length &w, const Length &h) {
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    GLint vw = viewport[2], vh = viewport[3] ;

    if ( w.units() == Length::Pixels )
        w_ = w.value() ;
    else if ( w.units() == Length::Percentage ) {
        if ( parent_ )
            w_ = parent_->width() * w.value() / 100.0 ;
        else
            w_ = vw * w.value() / 100.0 ;
    }

    if ( h.units() == Length::Pixels )
        h_ = h.value() ;
    else if ( h.units() == Length::Percentage ) {
        if ( parent_ )
            h_ = parent_->height() * h.value() / 100.0 ;
        else
            h_ = vh * h.value() / 100.0 ;
    }
}

void OverlayContainer::updateLayout() {
    if ( parent_ ) parent_->updateLayout() ;
    else layout() ;
}


void OverlayContainer::setMargins(float left, float top, float right, float bottom) {
    margins_ = Margins(left, top, right, bottom) ;

}

}
