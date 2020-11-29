#include <xviz/brush.hpp>

#include <style.pb.h>
#include <session.pb.h>

namespace xviz {


class BrushData {
public:

    FillRule fillRule() const { return fill_rule_ ; }

    void setFillRule(FillRule fill) {
        fill_rule_ = fill ;
    }

protected:

    FillRule fill_rule_ = FillRule::EvenOdd ;
};

class SolidBrushData: public BrushData {
public:
    SolidBrushData(const Color &clr): clr_(clr)  {}
    Color color() const { return clr_ ; }

    Color clr_ ;
};

class GradientBrushData: public BrushData {
public:
    void setTransform(const Matrix2d &mat) { mat_ = mat ; }

    Matrix2d mat_ ;
};

class LinearGradientBrushData: public GradientBrushData {
public:
    LinearGradientBrushData(const LinearGradient &lg): lg_(lg) {}

    LinearGradient lg_ ;
};

class RadialGradientBrushData: public GradientBrushData {
public:
    RadialGradientBrushData(const RadialGradient &rg): rg_(rg) {}

    RadialGradient rg_ ;
};

template<class T>
struct BrushDataDeleter
{
    void operator ()(const T *d)  {
        delete d ;
    }
};



Brush::Brush(): type_(BrushType::NoBrush) {
}

Brush::Brush(const Color &clr): type_(BrushType::SolidBrush) {
    data_.reset(new SolidBrushData(clr)) ;
}

Brush::Brush(const LinearGradient &lg): type_(BrushType::LinearGradientBrush) {
    new LinearGradientBrushData(lg) ;
    data_.reset(new LinearGradientBrushData(lg)) ;
}

Brush::Brush(const RadialGradient &lg): type_(BrushType::RadialGradientBrush) {
    data_.reset(new RadialGradientBrushData(lg)) ;
}

BrushType Brush::type() const {
    return type_ ;
}

Color Brush::color() const {
    if ( type() == BrushType::SolidBrush ) {
        return static_cast<const SolidBrushData *>(data_.get())->color() ;
    } else {
        return Color::white() ;
    }
}

void Brush::setFillRule(FillRule rule) {
    if ( type_ != BrushType::NoBrush ) {
        return data_->setFillRule(rule) ;
    }
}

FillRule Brush::fillRule() const {
    if ( type_ != BrushType::NoBrush ) {
        return data_->fillRule() ;
    } else {
        return FillRule::EvenOdd ;
    }
}


const LinearGradient &Brush::linearGradient() const {
    assert( type_ == BrushType::LinearGradientBrush ) ;
    return static_cast<const LinearGradientBrushData *>(data_.get())->lg_ ;
}

const RadialGradient &Brush::radialGradient() const {
    assert( type_ == BrushType::RadialGradientBrush ) ;
    return static_cast<const RadialGradientBrushData *>(data_.get())->rg_ ;
}

static msg::Gradient *write_gradient(const Gradient &g) {

    msg::Gradient *msg_g = new msg::Gradient() ;

    switch ( g.spread() ) {
    case SpreadMethod::Pad:
        msg_g->set_spread(msg::Gradient_SpreadMethod_PAD_SPREAD);
        break ;
    case SpreadMethod::Repeat:
        msg_g->set_spread(msg::Gradient_SpreadMethod_REPEAT_SPREAD);
        break ;
    case SpreadMethod::Reflect:
        msg_g->set_spread(msg::Gradient_SpreadMethod_REFLECT_SPREAD);
        break ;
    }

    switch ( g.units() ) {
    case GradientUnits::ObjectBoundingBox:
        msg_g->set_units(msg::Gradient_GradientUnits_OBJECT_BOUNDING_BOX);
        break ;
    case GradientUnits::UserSpaceOnUse:
        msg_g->set_units(msg::Gradient_GradientUnits_USER_SPACE_ON_USE);
        break ;
    }

    const auto &tr = g.transform() ;

    msg::Matrix2d *mat = new msg::Matrix2d ;
    mat->set_m1(tr.m1()) ;
    mat->set_m2(tr.m2()) ;
    mat->set_m3(tr.m3()) ;
    mat->set_m4(tr.m4()) ;
    mat->set_m5(tr.m5()) ;
    mat->set_m6(tr.m6()) ;
    msg_g->set_allocated_transform(mat) ;

    for( auto &stop: g.stops() ) {
        msg::Stop *msg_stop = msg_g->add_stops() ;
        msg_stop->set_pos(stop.position()) ;
        msg_stop->set_allocated_color(Color::write(stop.color())) ;
    }

    return msg_g ;

}

msg::Brush *Brush::write(const Brush &brush) {
    msg::Brush *msg_brush = new msg::Brush() ;

    switch ( brush.fillRule() ) {
    case FillRule::EvenOdd:
        msg_brush->set_fill_rule(msg::Brush_FillRule_EVEN_ODD);
        break ;
    case FillRule::NonZero:
        msg_brush->set_fill_rule(msg::Brush_FillRule_NON_ZERO);
        break ;
    }

    if ( brush.type() == BrushType::SolidBrush  ) {
        msg::SolidBrush *msg_sb = new msg::SolidBrush() ;
        msg_sb->set_allocated_color(Color::write(brush.color()));
        msg_brush->set_allocated_solid_brush(msg_sb);
    } else if ( brush.type() == BrushType::LinearGradientBrush ) {
        msg::LinearGradient *msg_lg = new msg::LinearGradient() ;

        const LinearGradient &lg = brush.linearGradient() ;

        msg::Gradient *msg_g = write_gradient(lg) ;

        msg_lg->set_allocated_common(msg_g) ;
        msg_lg->set_x1(lg.startPoint().x()) ;
        msg_lg->set_y1(lg.startPoint().y()) ;
        msg_lg->set_x2(lg.finishPoint().x()) ;
        msg_lg->set_y2(lg.finishPoint().y()) ;

        msg_brush->set_allocated_linear_gradient_brush(msg_lg);
    } else if ( brush.type() == BrushType::RadialGradientBrush ) {
        msg::RadialGradient *msg_rg = new msg::RadialGradient() ;

        const RadialGradient &rg = brush.radialGradient() ;

        msg::Gradient *msg_g = write_gradient(rg) ;

        msg_rg->set_allocated_common(msg_g) ;
        msg_rg->set_cx(rg.center().x()) ;
        msg_rg->set_cy(rg.center().y()) ;
        msg_rg->set_fx(rg.focal().x()) ;
        msg_rg->set_fy(rg.focal().y()) ;
        msg_rg->set_r(rg.radius()) ;


        msg_brush->set_allocated_radial_gradient_brush(msg_rg);
    }

    return msg_brush ;
}

static void set_brush_base(Brush &b, const msg::Brush &msg) {

    switch ( msg.fill_rule() ) {
    case msg::Brush_FillRule_EVEN_ODD:
        b.setFillRule(FillRule::EvenOdd) ;
        break ;
    case msg::Brush_FillRule_NON_ZERO:
        b.setFillRule(FillRule::NonZero) ;
        break ;
    }

}

static void read_gradient(Gradient &g, const msg::Gradient &msg) {

    switch ( msg.spread() ) {
    case msg::Gradient_SpreadMethod_PAD_SPREAD:
        g.setSpread(SpreadMethod::Pad);
        break ;
    case msg::Gradient_SpreadMethod_REPEAT_SPREAD:
        g.setSpread(SpreadMethod::Repeat);
        break ;
    case msg::Gradient_SpreadMethod_REFLECT_SPREAD:
        g.setSpread(SpreadMethod::Reflect);
        break ;
    }

    switch ( msg.units() ) {
    case msg::Gradient_GradientUnits_OBJECT_BOUNDING_BOX:
        g.setUnits(GradientUnits::ObjectBoundingBox);
        break ;
    case msg::Gradient_GradientUnits_USER_SPACE_ON_USE:
        g.setUnits(GradientUnits::UserSpaceOnUse);
        break ;
    }

    const msg::Matrix2d &mat = msg.transform() ;
    Matrix2d tr(mat.m1(), mat.m2(), mat.m3(), mat.m4(), mat.m5(), mat.m6());
    g.setTransform(tr);

    std::vector<GradientStop> stops ;

    for( auto &msg_stop: msg.stops() ) {
        stops.emplace_back(msg_stop.pos(), Color::read(msg_stop.color()));
    }

    g.setStops(stops) ;


}

Brush Brush::read(const msg::Brush &brush_msg) {
    if ( brush_msg.has_solid_brush() ) {
        const msg::SolidBrush &msg_sb = brush_msg.solid_brush() ;
        Brush b(Color::read(msg_sb.color())) ;
        set_brush_base(b, brush_msg) ;
        return b ;
    } else if ( brush_msg.has_linear_gradient_brush() ) {
        const msg::LinearGradient &msg_lg = brush_msg.linear_gradient_brush() ;
        double x1 = msg_lg.x1() ;
        double y1 = msg_lg.y1() ;
        double x2 = msg_lg.x2() ;
        double y2 = msg_lg.y2() ;

        LinearGradient lg(x1, y1, x2, y2, {}) ;
        read_gradient(lg, msg_lg.common()) ;

        Brush b(lg) ;
        set_brush_base(b, brush_msg) ;
        return b ;

    } else if ( brush_msg.has_radial_gradient_brush() ) {
        const msg::RadialGradient &msg_rg = brush_msg.radial_gradient_brush() ;
        double cx = msg_rg.cx() ;
        double cy = msg_rg.cy() ;
        double fx = msg_rg.fx() ;
        double fy = msg_rg.fy() ;
        double r = msg_rg.r() ;

        RadialGradient rg(cx, cy, fx, fy, r, {}) ;
        read_gradient(rg, msg_rg.common()) ;
        Brush b(rg) ;
        set_brush_base(b, brush_msg) ;
        return b ;
    } else {
        Brush b ;
        set_brush_base(b, brush_msg) ;
        return b ;
    }
}



}
