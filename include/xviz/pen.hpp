#ifndef XVIZ_PEN_HPP
#define XVIZ_PEN_HPP

#include <xviz/color.hpp>
#include <vector>
#include <memory>

namespace xviz {

namespace msg {
    class Pen ;
}

enum class LineJoin { Miter, Round, Bevel } ;
enum class LineCap { Butt, Round, Square } ;
enum LineStyle { SolidLine, DashLine, DotLine, DashDotLine, CustomDashLine } ;
enum PenType { NoPen, SolidPen } ;

class PenData ;
class Pen {
public:

    Pen(PenType t = NoPen) ;
    Pen( const Color &clr, double line_width = 1.0, LineStyle style = SolidLine ) ;

    PenType type() const { return type_ ; }

    Pen &setColor(const Color &brush) ;
    Pen &setLineWidth(double width) ;
    Pen &setMiterLimit(double limit) ;
    Pen &setLineJoin(LineJoin join) ;
    Pen &setLineCap(LineCap cap) ;
    Pen &setDashArray(const std::vector<double> &) ;
    Pen &setDashOffset(double offset) ;
    Pen &setLineStyle(LineStyle dash) ;

    LineJoin lineJoin() const ;
    LineCap lineCap() const ;
    LineStyle lineStyle() const ;
    Color lineColor() const ;
    double lineWidth() const ;
    double miterLimit() const ;
    const std::vector<double> &dashArray() const ;
    double dashOffset() const ;

    static Pen read(const msg::Pen &pen) ;
    static msg::Pen *write(const Pen &pen) ;


private:
     std::shared_ptr<PenData> data_ ;
     PenType type_ ;
} ;


}

#endif
