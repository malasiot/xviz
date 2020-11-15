#ifndef XVIZ_BRUSH_HPP
#define XVIZ_BRUSH_HPP

#include <xviz/color.hpp>
#include <xviz/gradient.hpp>

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

    const LinearGradient &linearGradient() const ;
    const RadialGradient &radialGradient() const ;

    static msg::Brush *write(const Brush &) ;
    static Brush read(const msg::Brush &) ;

protected:

    friend class BrushData ;

    std::shared_ptr<BrushData> data_ ;
    BrushType type_ ;
} ;

/*

class GradientBrush: public Brush {

public:

    void setSpread(SpreadMethod method) { sm_ = method ; }
  //  void setTransform(const Matrix2d &trans) { tr_ = trans ; }

    struct Stop {
        Stop(double offset, const Color &clr): offset_(offset), clr_(clr) {}
        double offset_ ;
        Color clr_ ;
    } ;

    GradientBrush &addStop(double offset, const Color &clr) {
        stops_.push_back(Stop(offset, clr)) ;
        return *this ;
    }

    SpreadMethod spread() const { return sm_ ; }
  //  const Matrix2d &transform() const { return tr_ ; }
    const std::vector<Stop> &stops() const { return stops_ ; }

protected:

    GradientBrush(): sm_(SpreadMethod::Pad) {}

private:

    std::vector<Stop> stops_ ;
    SpreadMethod sm_ ;
    //Matrix2d tr_ ;
} ;


class LinearGradientBrush: public GradientBrush {
public:

    LinearGradientBrush(double x0, double y0, double x1, double y1):
        x0_(x0), y0_(y0), x1_(x1), y1_(y1) {}

    double x0() const { return x0_ ; }
    double y0() const { return y0_ ; }
    double x1() const { return x1_ ; }
    double y1() const { return y1_ ; }

private:

    double x0_, y0_, x1_, y1_ ;
} ;

class RadialGradientBrush: public GradientBrush {

public:

    RadialGradientBrush(double cx, double cy, double r, double fx = 0, double fy = 0):
        cx_(cx), cy_(cy), fx_(fx), fy_(fy), r_(r)  {}

    double cx() const { return cx_ ; }
    double cy() const { return cy_ ; }
    double fx() const { return fx_ ; }
    double fy() const { return fy_ ; }
    double radius() const { return r_ ; }

private:

    double cx_, cy_, fx_, fy_, r_ ;
} ;

using BrushHandle = std::shared_ptr<Brush> ;
*/

/*
class PatternBrush: public Brush {

public:

    PatternBrush(const Surface &pattern):  pattern_(pattern), sm_(SpreadMethod::Pad)  {}

    std::unique_ptr<BrushBase> clone() const override { return std::unique_ptr<BrushBase>(new PatternBrush(*this)) ; }

    void setTransform(const Matrix2d &trans) { tr_ = trans ; }
    void setSpread(SpreadMethod method) { sm_ = method ; }

    Matrix2d transform() const { return tr_ ; }
    const Surface &pattern() const { return pattern_ ; }
    SpreadMethod spread() const { return sm_ ; }

private:

    SpreadMethod sm_ ;
    const Surface &pattern_ ;
    Matrix2d tr_ ;
} ;
*/
}

#endif
