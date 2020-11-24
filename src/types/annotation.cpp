#include <xviz/annotation.hpp>

#include "drawable.pb.h"

namespace xviz {

class AnnotationData {
public:
    AnnotationData(Annotation::Type type): type_(type) {}

    Annotation::Type type_ ;
};

class ShapeAnnotationData: public AnnotationData {
public:
    ShapeAnnotationData(): AnnotationData(Annotation::Shape) {}

    void addShape(const Path &p) { shapes_.push_back(p); }

    std::vector<Path> shapes_ ;
    Brush brush_ ;
    Pen pen_ ;
};

class MarkerAnnotationData: public AnnotationData {
public:
    MarkerAnnotationData(const Path &marker): AnnotationData(Annotation::Marker), marker_(marker) {}

    void addPosition(float x, float y) { positions_.emplace_back(x, y) ; }

    std::vector<Vector2d> positions_ ;
    Path marker_ ;

    Brush brush_ ;
    Pen pen_ ;
};


class LabelAnnotationData: public AnnotationData {
public:
    LabelAnnotationData(): AnnotationData(Annotation::Label) {}

    void addLabel(const std::string &l, float x, float y) {
        positions_.emplace_back(x, y) ;
        labels_.emplace_back(l) ;
    }

    std::vector<Vector2d> positions_ ;
    std::vector<std::string> labels_ ;

    Brush brush_ ;
    Pen pen_ ;
    FontHandle font_ ;
    int flags_ = LabelAnnotation::TextAlignHCenter |  LabelAnnotation::TextAlignVCenter ;
    float ox_ = 0.f, oy_ = 0.f ;
};

Annotation::Type Annotation::type() const {
    assert(data_) ;
    return data_->type_ ;
}

msg::Annotation *Annotation::write(const Annotation &d) {
    msg::Annotation *msg = new msg::Annotation ;

    if ( d.data_->type_ == Annotation::Shape ) {
        msg::ShapeAnnotation *shape_msg = new msg::ShapeAnnotation ;
        msg->set_allocated_shape(shape_msg) ;
        const ShapeAnnotationData *data = reinterpret_cast<const ShapeAnnotationData *>(d.data_.get()) ;
        for( const Path &p: data->shapes_ )
            shape_msg->mutable_shapes()->AddAllocated(Path::write(p)) ;
        shape_msg->set_allocated_brush(Brush::write(data->brush_)) ;
        shape_msg->set_allocated_pen(Pen::write(data->pen_)) ;
    } else if ( d.data_->type_ == Annotation::Label ) {
        msg::LabelAnnotation *label_msg = new msg::LabelAnnotation ;
        msg->set_allocated_label(label_msg) ;
        const LabelAnnotationData *data = reinterpret_cast<const LabelAnnotationData *>(d.data_.get()) ;
        for( const std::string &l: data->labels_ ) {
            std::string *label = label_msg->add_labels() ;
            label->assign(l) ;
        }
        for( const Vector2d &p: data->positions_ ) {
            msg::Point2 *pos = label_msg->add_positions() ;
            pos->set_x(p.x()) ; pos->set_y(p.y()) ;
        }
        label_msg->set_allocated_brush(Brush::write(data->brush_)) ;
        label_msg->set_allocated_pen(Pen::write(data->pen_)) ;
        if ( data->font_ )
            label_msg->set_allocated_font(Font::write(data->font_)) ;
        label_msg->set_ox(data->ox_) ;
        label_msg->set_oy(data->oy_) ;
        label_msg->set_flags(data->flags_) ;
    } else if ( d.data_->type_ == Annotation::Marker ) {
        msg::MarkerAnnotation *marker_msg = new msg::MarkerAnnotation ;
        msg->set_allocated_marker(marker_msg) ;
        const MarkerAnnotationData *data = reinterpret_cast<const MarkerAnnotationData *>(d.data_.get()) ;
        marker_msg->set_allocated_marker(Path::write(data->marker_)) ;
        marker_msg->set_allocated_brush(Brush::write(data->brush_)) ;
        marker_msg->set_allocated_pen(Pen::write(data->pen_)) ;
        for( const auto &pos: data->positions_ ) {
            msg::Point2 *p = marker_msg->add_positions() ;
            p->set_x(pos.x()) ;
            p->set_y(pos.y()) ;
        }
    }

    return msg ;
}

Annotation Annotation::read(const msg::Annotation &msg) {
    if ( msg.has_shape() ) {
        const msg::ShapeAnnotation &sa_msg = msg.shape() ;
        ShapeAnnotation sa ;
        for (const auto &shape: sa_msg.shapes() ) {
            sa.addShape(Path::read(shape)) ;
        }
        sa.setBrush(Brush::read(sa_msg.brush())) ;
        sa.setPen(Pen::read(sa_msg.pen())) ;
        return sa ;
    } else if ( msg.has_label() ) {
        const msg::LabelAnnotation &la_msg = msg.label() ;
        LabelAnnotation la ;
        la.setAlignFlags(la_msg.flags()) ;
        la.setOffset(la_msg.ox(), la_msg.oy()) ;
        la.setFont(Font::read(la_msg.font())) ;
        la.setBrush(Brush::read(la_msg.brush())) ;
        la.setPen(Pen::read(la_msg.pen())) ;

        for ( uint i=0 ; i<la_msg.labels_size() ; i++ ) {
            const msg::Point2 &p = la_msg.positions(i) ;
            la.addLabel(la_msg.labels(i), p.x(), p.y()) ;
        }

        return la ;
    } else if ( msg.has_marker() ) {
        const msg::MarkerAnnotation &ma_msg = msg.marker() ;
        MarkerAnnotation ma(Path::read(ma_msg.marker())) ;
        for (const auto &position: ma_msg.positions() ) {
            ma.addPosition(position.x(), position.y()) ;
        }
        ma.setBrush(Brush::read(ma_msg.brush())) ;
        ma.setPen(Pen::read(ma_msg.pen())) ;

        return ma ;
    }
}

MarkerAnnotation::MarkerAnnotation(const Path &marker) {
    data_.reset(new MarkerAnnotationData(marker)) ;
}

MarkerAnnotation &MarkerAnnotation::setBrush(Brush b) {
    MarkerAnnotationData *data = static_cast<MarkerAnnotationData *>(data_.get()) ;
    data->brush_ = b ;
    return *this ;
}

MarkerAnnotation &MarkerAnnotation::setPen(Pen p) {
    MarkerAnnotationData *data = static_cast<MarkerAnnotationData *>(data_.get()) ;
    data->pen_ = p ;
    return *this ;
}

MarkerAnnotation &MarkerAnnotation::addPosition(float x, float y) {
    MarkerAnnotationData *data = static_cast<MarkerAnnotationData *>(data_.get()) ;
    data->positions_.emplace_back(x, y) ;
    return *this ;
}

Brush MarkerAnnotation::brush() const {
    const MarkerAnnotationData *data = static_cast<const MarkerAnnotationData *>(data_.get()) ;
    return data->brush_ ;
}

Pen MarkerAnnotation::pen() const {
    const MarkerAnnotationData *data = static_cast<const MarkerAnnotationData *>(data_.get()) ;
    return data->pen_ ;
}

Path MarkerAnnotation::marker() const {
    const MarkerAnnotationData *data = static_cast<const MarkerAnnotationData *>(data_.get()) ;
    return data->marker_ ;
}

const std::vector<Vector2d> &MarkerAnnotation::positions() const {
    const MarkerAnnotationData *data = static_cast<const MarkerAnnotationData *>(data_.get()) ;
    return data->positions_ ;
}


ShapeAnnotation::ShapeAnnotation() {
    data_.reset(new ShapeAnnotationData()) ;
}

const std::vector<Path> &ShapeAnnotation::shapes() const {
    const ShapeAnnotationData *data = static_cast<const ShapeAnnotationData *>(data_.get()) ;
    return data->shapes_ ;
}

ShapeAnnotation &ShapeAnnotation::setBrush(Brush b) {
    ShapeAnnotationData *data = static_cast<ShapeAnnotationData *>(data_.get()) ;
    data->brush_ = b ;
    return *this ;
}

ShapeAnnotation &ShapeAnnotation::setPen(Pen p) {
    ShapeAnnotationData *data = static_cast<ShapeAnnotationData *>(data_.get()) ;
    data->pen_ = p ;
    return *this ;
}

Pen ShapeAnnotation::pen() const {
   const ShapeAnnotationData *data = static_cast<const ShapeAnnotationData *>(data_.get()) ;
   return data->pen_ ;
}

Brush ShapeAnnotation::brush() const {
   const ShapeAnnotationData *data = static_cast<const ShapeAnnotationData *>(data_.get()) ;
   return data->brush_ ;
}

ShapeAnnotation &ShapeAnnotation::addShape(const Path &path) {
    ShapeAnnotationData *data = static_cast<ShapeAnnotationData *>(data_.get()) ;
    data->shapes_.push_back(path) ;
    return *this ;
}


LabelAnnotation::LabelAnnotation() {
    data_.reset(new LabelAnnotationData()) ;
}

LabelAnnotation &LabelAnnotation::setOffset(float ox, float oy) {
    LabelAnnotationData *data = static_cast<LabelAnnotationData *>(data_.get()) ;
    data->ox_ = ox ;
    data->oy_ = oy ;
    return *this ;
}

Brush LabelAnnotation::brush() const {
    const LabelAnnotationData *data = static_cast<const LabelAnnotationData *>(data_.get()) ;
    return data->brush_ ;
}

Pen LabelAnnotation::pen() const {
    const LabelAnnotationData *data = static_cast<const LabelAnnotationData *>(data_.get()) ;
    return data->pen_ ;
}

FontHandle LabelAnnotation::font() const {
    const LabelAnnotationData *data = static_cast<const LabelAnnotationData *>(data_.get()) ;
    return data->font_ ;
}

int LabelAnnotation::alignFlags() const {
    const LabelAnnotationData *data = static_cast<const LabelAnnotationData *>(data_.get()) ;
    return data->flags_ ;
}

Vector2d LabelAnnotation::offset() const {
    const LabelAnnotationData *data = static_cast<const LabelAnnotationData *>(data_.get()) ;
    return Vector2d(data->ox_, data->oy_) ;
}
const std::vector<std::string> & LabelAnnotation::labels() const {
    const LabelAnnotationData *data = static_cast<const LabelAnnotationData *>(data_.get()) ;
    return data->labels_ ;
}

const std::vector<Vector2d> & LabelAnnotation::positions() const {
    const LabelAnnotationData *data = static_cast<const LabelAnnotationData *>(data_.get()) ;
    return data->positions_ ;
}


LabelAnnotation &LabelAnnotation::addLabel(const std::string &text, float x, float y)
{
    LabelAnnotationData *data = static_cast<LabelAnnotationData *>(data_.get()) ;
    data->labels_.push_back(text) ;
    data->positions_.emplace_back(x, y) ;
    return *this ;
}

LabelAnnotation &LabelAnnotation::setBrush(Brush b) {
    LabelAnnotationData *data = static_cast<LabelAnnotationData *>(data_.get()) ;
    data->brush_ = b ;
    return *this ;
}

LabelAnnotation &LabelAnnotation::setFont(FontHandle f) {
    LabelAnnotationData *data = static_cast<LabelAnnotationData *>(data_.get()) ;
    data->font_ = f ;
    return *this ;
}

LabelAnnotation &LabelAnnotation::setPen(Pen p) {
    LabelAnnotationData *data = static_cast<LabelAnnotationData *>(data_.get()) ;
    data->pen_ = p ;
    return *this ;
}

LabelAnnotation &LabelAnnotation::setAlignFlags(int align) {
    LabelAnnotationData *data = static_cast<LabelAnnotationData *>(data_.get()) ;
    data->flags_ = align ;
    return *this ;
}



}
