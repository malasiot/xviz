#include <xviz/overlay/canvas.hpp>

#include "common/gl/gl3w.h"
#include "3rdparty/nanovg/nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "3rdparty/nanovg/nanovg_gl.h"

namespace xviz {

Canvas::Canvas() {
    vg_ = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES );

    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    GLint vw = viewport[2], vh = viewport[3] ;

    nvgBeginFrame(vg_, vw, vh, 1.0);
}

Canvas::~Canvas() {
    nvgEndFrame(vg_);
    nvgDeleteGL3(vg_) ;
}

void Canvas::save() {
    nvgSave(vg_) ;
}

void Canvas::restore() {
    nvgRestore(vg_) ;
}

void Canvas::setStrokeColor(float r, float g, float b, float a) {
    nvgStrokeColor(vg_, nvgRGBAf(r, g, b, a));
}

void Canvas::setStrokeWidth(float w) {
    nvgStrokeWidth(vg_, w) ;
}

void Canvas::setMiterLimit(float limit) {
    nvgMiterLimit(vg_, limit) ;
}

void Canvas::setLineCap(LineCap lc) {
    switch ( lc ) {
    case LineCapButt:
        nvgLineCap(vg_, NVG_BUTT) ;
        break;
    case LineCapRound:
        nvgLineCap(vg_, NVG_ROUND) ;
        break;
    default:
        nvgLineCap(vg_, NVG_SQUARE) ;
        break;
    }
}

void Canvas::setLineJoin(LineJoin lj) {
    switch ( lj ) {
    case LineJoinMiter:
        nvgLineJoin(vg_, NVG_MITER) ;
        break;
    case LineJoinRound:
        nvgLineJoin(vg_, NVG_ROUND) ;
        break;
    default:
        nvgLineJoin(vg_, NVG_BEVEL) ;
        break;
    }
}


void Canvas::setPaint(float r, float g, float b, float a) {
    nvgFillColor(vg_, nvgRGBAf(r, g, b, a));
}

void Canvas::beginPath() {
    nvgBeginPath(vg_) ;
}

void Canvas::moveTo(float x, float y) {
    nvgMoveTo(vg_, x, y) ;
}

void Canvas::lineTo(float x, float y) {
    nvgLineTo(vg_, x, y) ;
}

void Canvas::curveTo(float c1x, float c1y, float c2x, float c2y, float x, float y) {
    nvgBezierTo(vg_, c1x, c1y, c2x, c2y, x, y) ;
}

void Canvas::quadTo(float cx, float cy, float x, float y) {
    nvgQuadTo(vg_, cx, cy, x, y) ;
}

void Canvas::arcTo(float x1, float y1, float x2, float y2, float radius) {
    nvgArcTo(vg_, x1, y1, x2, y2, radius) ;
}

void Canvas::rect(float x, float y, float w, float h) {
    nvgRect(vg_, x, y, w, h) ;
}

void Canvas::closePath() {
    nvgClosePath(vg_) ;
}

void Canvas::roundedRect(float x, float y, float w, float h, float radius) {
    nvgRoundedRect(vg_, x, y, w, h, radius) ;
}

void Canvas::ellipse(float x, float y, float rx, float ry) {
    nvgEllipse(vg_, x, y, rx, ry) ;
}

void Canvas::circle(float x, float y, float rad) {
    nvgCircle(vg_, x, y, rad) ;
}

void Canvas::stroke() {
    nvgStroke(vg_) ;
}

void Canvas::fill() {
    nvgFill(vg_) ;
}


}
