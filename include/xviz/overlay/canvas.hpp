#pragma once

#include <xviz/overlay/overlay.hpp>

class NVGcontext ;

namespace xviz {


class Canvas {
public:
    Canvas(float w, float h) ;
    ~Canvas() ;
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
