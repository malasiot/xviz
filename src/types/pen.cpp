#include <xviz/pen.hpp>

#include <session.pb.h>

using namespace std ;

namespace xviz {

class PenData {

    friend class Pen ;

    double line_width_, miter_limit_ = 0 ;
    LineJoin line_join_ = LineJoin::Round ;
    LineCap line_cap_ = LineCap::Round ;
    LineStyle line_style_ = SolidLine ;
    std::vector<double> dash_array_ ;
    double dash_offset_ = 0.f;
    Color line_color_ ;
} ;

Pen & Pen::setColor(const Color &brush) {
    assert(data_);
    data_->line_color_ = brush ;
    return *this ;
}

Pen & Pen::setLineWidth(double width) {
    assert(data_) ;
    data_->line_width_ = width ;
    return *this ;

}

Pen & Pen::setMiterLimit(double limit) {
    assert(data_) ;
    data_->miter_limit_ = limit ;
    return *this ;
}

Pen & Pen::setLineJoin(LineJoin join) {
    assert(data_) ;
    data_->line_join_ = join ;
    return *this ;
}

Pen & Pen::setLineCap(LineCap cap) {
    assert(data_) ;
    data_->line_cap_ = cap ;
    return *this ;
}

Pen & Pen::setDashArray(const vector<double> &dashes) {
    assert(data_) ;
    data_->dash_array_ = dashes ;
    return *this ;
}

Pen & Pen::setDashOffset(double offset) {
    assert(data_) ;
    data_->dash_offset_ = offset ;
    return *this ;
}

Pen & Pen::setLineStyle(LineStyle style) {
    assert( data_ ) ;
    data_->line_style_ = style ;
    return *this ;
}

LineJoin Pen::lineJoin() const {
    assert(data_) ;
    return data_->line_join_ ;
}

LineCap Pen::lineCap() const {
    assert(data_) ;
    return data_->line_cap_ ;
}

double Pen::lineWidth() const {
    assert(data_) ;
    return data_->line_width_ ;
}

double Pen::miterLimit() const {
    assert(data_) ;
    return data_->miter_limit_ ;
}

const std::vector<double> &Pen::dashArray() const {
    assert(data_) ;
    return data_->dash_array_ ;
}

double Pen::dashOffset() const {
    assert(data_) ;
    return data_->dash_offset_ ;
}

LineStyle Pen::lineStyle() const {
    assert(data_) ;
    return data_->line_style_ ;
}

Color Pen::lineColor() const {
    assert(data_) ;
    return data_->line_color_ ;
}


Pen::Pen(const Color &clr, double width, LineStyle style): type_(SolidPen), data_(new PenData) {
    data_->line_color_ = clr ;
    data_->line_style_ = style ;
    data_->line_width_ = width ;
}


Pen Pen::read(const msg::Pen &pen) {
    if ( pen.has_empty_pen() )
        return Pen() ;
    if ( pen.has_solid_pen() ) {
        Pen sp(SolidPen) ;
        const msg::SolidPen &msg_pen = pen.solid_pen();

        sp.setLineWidth(msg_pen.line_width());

        switch ( msg_pen.stroke_type() ) {
        case msg::SolidPen_StrokeType_SOLID:
            sp.setLineStyle(SolidLine) ;
            break ;
        case msg::SolidPen_StrokeType_DASHED:
            sp.setLineStyle(DashLine) ;
            break ;
        case msg::SolidPen_StrokeType_DOTTED:
            sp.setLineStyle(DotLine) ;
            break ;
        case msg::SolidPen_StrokeType_DASH_DOT:
            sp.setLineStyle(DashDotLine) ;
            break ;
        case msg::SolidPen_StrokeType_CUSTOM:
            sp.setLineStyle(CustomDashLine) ;
            break ;
        }

        switch ( msg_pen.line_join() ) {
        case msg::SolidPen_LineJoin_MITER_JOIN:
            sp.setLineJoin(LineJoin::Miter) ;
            break ;
        case msg::SolidPen_LineJoin_ROUND_JOIN:
            sp.setLineJoin(LineJoin::Round) ;
            break ;
        case msg::SolidPen_LineJoin_BEVEL_JOIN:
            sp.setLineJoin(LineJoin::Bevel) ;
            break ;
        }

        switch ( msg_pen.line_cap() ) {
        case msg::SolidPen_LineCap_BUTT_CAP:
            sp.setLineCap(LineCap::Butt) ;
            break ;
        case msg::SolidPen_LineCap_SQUARE_CAP:
            sp.setLineCap(LineCap::Square) ;
            break ;
        case msg::SolidPen_LineCap_ROUND_CAP:
            sp.setLineCap(LineCap::Round) ;
            break ;
        }

        sp.setDashOffset(msg_pen.dash_offset()) ;

        vector<double> dashes ;

        for( const float &v: msg_pen.dash_array() )
            dashes.push_back(v) ;

        sp.setDashArray(dashes) ;

        if ( msg_pen.has_line_color() )
            sp.setColor(Color::read(msg_pen.line_color()));

        return sp ;
    }
}

Pen::Pen(PenType t): type_(t) {
    if ( type_ == SolidPen )
        data_.reset(new PenData()) ;
}

msg::Pen *Pen::write(const Pen &pen)
{
    msg::Pen *msg_pen = new msg::Pen() ;

    if ( pen.type() == NoPen ) {
        msg_pen->set_allocated_empty_pen(new msg::EmptyPen);
    } else if ( pen.type() == SolidPen ) {
        msg::SolidPen *sp = new msg::SolidPen() ;
        sp->set_line_width(pen.lineWidth()) ;
        sp->set_allocated_line_color(Color::write(pen.lineColor())) ;

        switch ( pen.lineStyle() ) {
        case SolidLine:
            sp->set_stroke_type(msg::SolidPen_StrokeType_SOLID) ;
            break ;
        case DashLine:
            sp->set_stroke_type(msg::SolidPen_StrokeType_DASHED) ;
            break ;
        case DashDotLine:
            sp->set_stroke_type(msg::SolidPen_StrokeType_DASH_DOT) ;
            break ;
        case DotLine:
            sp->set_stroke_type(msg::SolidPen_StrokeType_DOTTED) ;
            break ;
        case CustomDashLine:
            sp->set_stroke_type(msg::SolidPen_StrokeType_CUSTOM) ;
            break ;
        }

        switch ( pen.lineJoin() ) {
        case LineJoin::Bevel:
            sp->set_line_join(msg::SolidPen_LineJoin_BEVEL_JOIN) ;
            break ;
        case LineJoin::Miter:
            sp->set_line_join(msg::SolidPen_LineJoin_MITER_JOIN) ;
            break ;
        case LineJoin::Round:
            sp->set_line_join(msg::SolidPen_LineJoin_ROUND_JOIN) ;
            break ;
        }

        switch ( pen.lineCap() ) {
        case LineCap::Butt:
            sp->set_line_cap(msg::SolidPen_LineCap_BUTT_CAP) ;
            break ;
        case LineCap::Square:
            sp->set_line_cap(msg::SolidPen_LineCap_SQUARE_CAP) ;
            break ;
        case LineCap::Round:
            sp->set_line_cap(msg::SolidPen_LineCap_ROUND_CAP) ;
            break ;
        }

        sp->set_dash_offset(pen.dashOffset());

        for( const float &v: pen.dashArray() ) {
            sp->add_dash_array(v) ;
        }

        msg_pen->set_allocated_solid_pen(sp);
    }

    return msg_pen ;
}


}
