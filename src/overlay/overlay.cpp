#include <xviz/overlay/overlay.hpp>
#include "gl_rect.hpp"
using namespace Eigen ;

namespace xviz {

void OverlayItem::setPosition(float x, float y) {
    x_ = x ; y_ = y ;
}

void Frame::draw() {

    impl::OpenGLRect rect ;
    rect.render(x_ + margins_.left_ + bw_/2.0,
                y_ + margins_.top_ + bw_/2.0,
                w_ - margins_.left_ - margins_.right_ - bw_,
                h_ - margins_.top_ - margins_.bottom_ - bw_,
                fill_, stroke_, bw_, opacity_) ;
    if ( child_ ) child_->draw() ;
}

void Frame::layout() {
    if ( !child_ ) return ;

    child_->setPosition(x_ + margins_.left_ + bw_, y_ + margins_.top_ + bw_) ;
    child_->setSize(w_ - margins_.left_ - margins_.right_ - bw_ - bw_,
                    h_ - margins_.top_ - margins_.bottom_ - bw_ - bw_) ;
    child_->layout() ;
}

void Frame::measure(float &mw, float &mh) {

    float cw = 0, ch = 0 ;

    if ( child_ ) {
        child_->measure(cw, ch) ;
    }

    mw = cw + margins_.left_ + margins_.right_ + 2 * bw_ ;
    mh = ch + margins_.top_ + margins_.bottom_ + 2 * bw_ ;
}


void OverlayContainer::setMargins(float left, float top, float right, float bottom) {
    margins_ = Margins(left, top, right, bottom) ;

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

Frame::Frame(OverlayContainer *child) {
    if ( child ) {
        child_.reset(child) ;
        child_->setParent(this) ;
    }
}


}
