#include <xviz/overlay/frame.hpp>

#include "gl_rect.hpp"

namespace xviz {

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


Frame::Frame(OverlayContainer *child) {
    if ( child ) {
        child_.reset(child) ;
        child_->setParent(this) ;
    }
}


}
