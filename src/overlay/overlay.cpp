#include <xviz/overlay/overlay.hpp>
#include "gl_rect.hpp"
using namespace Eigen ;

namespace xviz {

void OverlayItem::setPosition(float x, float y) {
    x_ = x ; y_ = y ;
}

void Frame::draw() {

    impl::OpenGLRect rect ;
    rect.render(x_ + margins_.left_,
                y_ + margins_.top_,
                w_ - margins_.left_ - margins_.right_,
                h_ - margins_.top_ - margins_.bottom_,
                fill_, stroke_, bw_, opacity_) ;
    if ( child_ ) child_->draw() ;
}

void Frame::layout() {
    if ( !child_ ) return ;

    child_->setPosition(x_ + margins_.left_ + bw_/2, y_ + margins_.top_ + bw_/2) ;
    child_->setSize(w_ - margins_.left_ - margins_.right_ - bw_,
                    h_ - margins_.top_ - margins_.bottom_ - bw_) ;
    child_->layout() ;
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

std::pair<float, float> OverlayContainer::measure(float w, MeasureMode m_horz, float h, MeasureMode m_vert) {
    float mw, mh ;

    switch ( m_horz ) {
    case MeasureModeExact:
        mw = w ;
        break ;
    case MeasureModeUndefined:
         mh = ( max_width_ ) ? max_width_.value() : 0 ;
        break ;
    case MeasureModeAtMost:
        mw = ( max_width_ ) ? std::min(max_width_.value(), w) : w ;
        break ;
    }

    switch ( m_vert ) {
    case MeasureModeExact:
        mh = h ;
        break ;
    case MeasureModeUndefined:
        mh = ( max_height_ ) ? max_height_.value() : 0 ;
        break ;
    case MeasureModeAtMost:
        mh = ( max_height_ ) ? std::min(max_height_.value(), h) : h ;
        break ;
    }

    return std::make_pair(mw, mh) ;
}

Frame::Frame(OverlayContainer *child) {
    if ( child ) {
        child_.reset(child) ;
        child_->setParent(this) ;
    }
}


}
