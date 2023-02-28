#pragma once

#include <xviz/overlay/overlay.hpp>
#include <xviz/common/image.hpp>

namespace xviz {

namespace impl {
class OpenGLImage ;
}

class ImageBox: public OverlayContainer {
   public:

    enum Scaling { ScaleToFit, ScaleCrop, ScaleNone, ScaleStretch } ;
    enum Alignment { AlignCenter, AlignBottom, AlignTop, AlignLeft, AlignRight, AlignTopLeft, AlignTopRight, AlignBottomLeft, AlignBottomRight } ;

    ImageBox(const Image &image) ;

    void setScaling(Scaling scaling) { scaling_ = scaling ; }
    void setAlignment(Alignment align) { alignment_ = align ; }

    void layout() override ;

    void draw() override ;
    void measure(float &mw, float &mh) override ;
private:

    std::unique_ptr<impl::OpenGLImage> image_ ;
    float iw_, ih_, ox_, oy_ ;
    Scaling scaling_ = ScaleToFit ;
    Alignment alignment_ = AlignCenter;

};

}
