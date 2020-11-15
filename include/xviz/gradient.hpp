#ifndef XVIZ_GRADIENT_HPP
#define XVIZ_GRADIENT_HPP

#include <xviz/color.hpp>
#include <xviz/vector.hpp>

#include <vector>


namespace xviz {

enum SpreadMethod { PadSpread, RepeatSpread, ReflectSpread } ;

class GradientStop {
public:
    GradientStop(double pos, const Color &clr): pos_(pos), clr_(clr) {}

    Color color() const { return clr_ ; }
    double position() const { return pos_ ; }

protected:
    Color clr_ ;
    double pos_ ;
};

class Gradient {
protected:
    Gradient(const std::vector<GradientStop> &stops): stops_(stops), spread_(PadSpread) {}

public:
    void setSpread(SpreadMethod sp){ spread_ = sp ; }
    void setStops(const std::vector<GradientStop> &stops) { stops_ = stops ; }

    const std::vector<GradientStop> &stops() const { return stops_ ; }
    SpreadMethod spread() const { return spread_ ; }

protected:
    SpreadMethod spread_ ;
    std::vector<GradientStop> stops_ ;
};

class LinearGradient: public Gradient {
public:
    LinearGradient(const Point2d &p1, const Point2d &p2,
                   const std::vector<GradientStop> &stops): x1_(p1.x()), y1_(p1.y()), x2_(p2.x()), y2_(p2.y()), Gradient(stops) {}
    LinearGradient(double x1, double y1, double x2, double y2,
                   const std::vector<GradientStop> &stops): x1_(x1), y1_(y1), x2_(x2), y2_(y2), Gradient(stops) {}

    Point2d startPoint() const { return { x1_, y1_ } ; }
    Point2d finishPoint() const { return { x2_, y2_ } ; }

protected:

    double x1_, y1_, x2_, y2_ ;
};

class RadialGradient: public Gradient {
public:
    RadialGradient(double cx, double cy, double fx, double fy, double r,
                   const std::vector<GradientStop> &stops): cx_(cx), cy_(cy), fx_(fx), fy_(fy), r_(r), Gradient(stops) {}

    RadialGradient(const Point2d &center, const Point2d &focal, double r,
                   const std::vector<GradientStop> &stops): cx_(center.x()), cy_(center.y()), fx_(focal.x()), fy_(focal.y()), r_(r), Gradient(stops) {}

    Point2d center() const { return { cx_, cy_ } ; }
    Point2d focal() const { return { fx_, fy_ } ; }
    double radius() const { return r_ ; }

protected:

    double cx_, cy_, fx_, fy_, r_ ;
};



}

#endif
