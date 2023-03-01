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
    void setLineJoin(LineJoin lj) ;

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
    void ellipse(float x, float y, float rx, float ry) ;
    void circle(float x, float y, float rad) ;

private:
    NVGcontext *vg_ ;
};


}
