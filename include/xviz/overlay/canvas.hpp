#pragma once

#include <xviz/overlay/overlay.hpp>

class NVGcontext ;

namespace xviz {


class Canvas {
public:

    enum LineCap { LineCapButt, LineCapRound, LineCapSquare } ;
    enum LineJoin { LineJoinMiter, LineJoinRound, LineJoinBevel } ;

    Canvas() ;
    ~Canvas() ;

    void save() ;
    void restore() ;
    void setStrokeColor(float r, float g, float b, float a=1.0) ;
    void setStrokeWidth(float w) ;
    void setMiterLimit(float limit);
    void setLineCap(LineCap lc) ;
    void setLineJoint(LineJoin lj) ;

    // Sets the stroke width of the stroke style.
    void nvgStrokeWidth(NVGcontext* ctx, float size);

    // Sets how the end of the line (cap) is drawn,
    // Can be one of: NVG_BUTT (default), NVG_ROUND, NVG_SQUARE.
    void nvgLineCap(NVGcontext* ctx, int cap);

    // Sets how sharp path corners are drawn.
    // Can be one of NVG_MITER (default), NVG_ROUND, NVG_BEVEL.
    void nvgLineJoin(NVGcontext* ctx, int join);
    void setPaint(float r, float g, float b, float a = 1.0) ;


    void stroke() ;
    void fill() ;

    void beginPath() ;
    void closePath() ;
    void moveTo(float x, float y) ;
    void lineTo(float x, float y) ;
    void curveTo(float c1x, float c1y, float c2x, float c2y, float x, float y);
    void quadTo(float cx, float cy, float x, float y);
    void arcTo(float x1, float y1, float x2, float y2, float radius);

    void rect(float x, float y, float w, float h) ;
    void roundedRect(float x, float y, float w, float h, float radius) ;

/*
    void newPath() ;
    void moveTo(float x, float y) ;
    void lineTo(float x, float y) ;

    void closePath() ;
  */
    void draw() ;


private:
    NVGcontext *vg_ ;
};


}
