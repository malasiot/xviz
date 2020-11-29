#ifndef XVIZ_COLORMAP_HPP
#define XVIZ_COLORMAP_HPP

#include <vector>
#include <array>

namespace xviz {

enum ColorMapType {
  ColorMapNone = 0,
  ColorMapHot = 1,
  ColorMapGray = 2,
  ColorMapMagma = 3

};

class ColorMap {
public:

    using color_t = std::array<float, 3> ;

    ColorMap() = delete ;

    static color_t interpolate(float v, int type) ;

};

}

#endif
