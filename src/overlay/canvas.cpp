#include <xviz/overlay/canvas.hpp>

#include "common/gl/gl3w.h"
#include "3rdparty/nanovg/nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "3rdparty/nanovg/nanovg_gl.h"

namespace xviz {

Canvas::Canvas(float w, float h) {
    vg_ = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES );
    nvgBeginFrame(vg_, w, h, 1.0);
}

Canvas::~Canvas() {

    nvgEndFrame(vg_);
    nvgDeleteGL3(vg_) ;
}


void Canvas::draw() {

    nvgSave(vg_);
    //	nvgClearState(vg);

        // Window
       nvgBeginPath(vg_);

       nvgMoveTo(vg_, 10, 10) ;
       nvgLineTo(vg_, 500, 500) ;
       nvgStroke(vg_) ;

       nvgBeginPath(vg_);
        nvgRoundedRect(vg_, 10, 10, 100, 50, 4);
        nvgFillColor(vg_, nvgRGBA(28,30,34,192));
    //	nvgFillColor(vg, nvgRGBA(0,0,0,128));
        nvgFill(vg_);
        nvgStrokeColor(vg_, nvgRGBA(100, 100, 0, 255));
        nvgStrokeWidth(vg_, 4) ;
        nvgStroke(vg_) ;
}

}
