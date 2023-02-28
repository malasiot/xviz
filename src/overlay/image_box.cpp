#include <xviz/overlay/image_box.hpp>

#include "gl_image.hpp"

namespace xviz {

ImageBox::ImageBox(const Image &image)
{
    image_.reset(new impl::OpenGLImage(image)) ;
}

void ImageBox::draw() {
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    GLint vw = viewport[2], vh = viewport[3] ;

    glScissor(x_ + margins_.left_,
              vh - h_ - y_ + margins_.top_,
              w_ - margins_.left_ - margins_.right_,
              h_ - margins_.top_ - margins_.bottom_);
    glEnable(GL_SCISSOR_TEST);
    image_->draw(x_ + ox_, y_ + oy_, iw_, ih_) ;
    glDisable(GL_SCISSOR_TEST);
}

void ImageBox::measure(float &mw, float &mh) {
    mw = margins_.left_ + margins_.right_ ;
    mh = margins_.top_ + margins_.bottom_ ;
}

void ImageBox::layout()
{
    float w = w_ - margins_.left_ - margins_.right_ ;
    float h = h_ - margins_.top_ - margins_.bottom_ ;

    int orig_width = image_->width() ;
    int orig_height = image_->height() ;
    float orig_aspect = orig_width/static_cast<float>(orig_height) ;
    float aspect = w / h ;

    ox_ = 0 ; oy_ = 0 ;
    if ( scaling_ == ScaleToFit ) {
        iw_ = w ;
        ih_ = h ;
        if ( orig_aspect > aspect ) {
            ih_ = w / orig_aspect ;
        }  else {
            iw_ = h * orig_aspect ;
        }
    } else if ( scaling_ == ScaleNone ) {
        iw_ = orig_width ;
        ih_ = orig_height ;
    } else if ( scaling_ == ScaleCrop ) {
        iw_ = w ;
        ih_ = h ;
        if ( orig_aspect < aspect ) {
            ih_ = w / orig_aspect ;
        }  else {
            iw_ = h * orig_aspect ;
        }

    } else if ( scaling_ == ScaleStretch ) {
        iw_ = w ;
        ih_ = h ;
    }

    switch ( alignment_ ) {
    case AlignTop:
        ox_ = (w - iw_)/2.0 ;
        break ;
    case AlignBottom:
        ox_ = (w - iw_)/2.0 ;
        oy_ = h - ih_ ;
        break ;
    case AlignCenter:
        ox_ = (w - iw_)/2.0 ;
        oy_ = (h - ih_)/2.0 ;
        break ;
    case AlignLeft:
        oy_ = (h - ih_)/2.0 ;
        break ;
    case AlignRight:
        oy_ = (h - ih_)/2.0 ;
        ox_ = w - iw_ ;
        break ;
    case AlignTopRight:
        ox_ = w - iw_ ;
        break ;
    case AlignBottomLeft:
        oy_ = h - ih_ ;
        break ;
    case AlignBottomRight:
        oy_ = h - ih_ ;
        ox_ = w - iw_ ;
        break ;
    default:
        break;
    }

    ox_ += margins_.left_ ;
    oy_ += margins_.bottom_ ;

}

}
