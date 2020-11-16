#ifndef XVIZ_BRUSH_HPP
#define XVIZ_BRUSH_HPP

#include <xviz/color.hpp>
#include <xviz/gradient.hpp>
#include <xviz/xform.hpp>

#include <vector>
#include <memory>

namespace xviz {

namespace msg {
    class Brush ;
}

enum class FillRule { EvenOdd, NonZero } ;
enum class BrushType { NoBrush, SolidBrush, LinearGradientBrush, RadialGradientBrush } ;

// abstract class for all brush type

class BrushData ;

class LinearGradient ;
class RadialGradient ;

class Brush {
public:

    Brush() ; // empty brush
    Brush(const Color &clr) ; // solid brush
    Brush(const LinearGradient &lg) ; // linear gradient brush
    Brush(const RadialGradient &rg) ; // radial gradient brush

    BrushType type() const ;
    Color color() const ;

    void setFillRule(FillRule rule) ;
    FillRule fillRule() const ;

    void setTransform(const Matrix2d &mat) ;
    Matrix2d transform() const ;

    const LinearGradient &linearGradient() const ;
    const RadialGradient &radialGradient() const ;

    static msg::Brush *write(const Brush &) ;
    static Brush read(const msg::Brush &) ;

protected:

    friend class BrushData ;

    std::shared_ptr<BrushData> data_ ;
    BrushType type_ ;
} ;

}

#endif
