#include <xviz/marker.hpp>

#include <session.pb.h>

namespace xviz {

class MarkerData {
public:
    MarkerData(double sz): sz_(sz) {}

    double sz_ ;
} ;

class StockMarkerShapeData: public MarkerData {
public:
    StockMarkerShapeData(StockMarkerShape shape, const Pen &pen, const Brush &brush, double sz):
        MarkerData(sz), shape_(shape), pen_(pen), brush_(brush) {}

    StockMarkerShape shape_ ;
    Pen pen_ ;
    Brush brush_ ;
};

/*
class CustomShapeMarkerData: public MarkerData {
public:
    CustomShapeMarkerData(const ShapeDrawable &d, double sz): MarkerData(sz), drawable_(d) {}

    ShapeDrawable drawable_ ;
};
*/
Marker::Marker(): type_(NoMarker)  {
}

Marker::Marker(StockMarkerShape shape, Pen pen, Brush brush, double sz): type_(StockShapeMarker), data_(new StockMarkerShapeData(shape, pen, brush, sz)) {

}

/*
Marker::Marker(const ShapeDrawable &d, double sz): type_(CustomShapeMarker),
    data_(new CustomShapeMarkerData(d, sz)) {
}
*/
double Marker::size() const {
    assert(data_) ;
    return data_->sz_ ;
}

Brush Marker::brush() const {
    assert(data_ && type_ == StockShapeMarker ) ;
    return static_cast<const StockMarkerShapeData *>(data_.get())->brush_ ;
}

Pen Marker::pen() const {
    assert(data_ && type_ == StockShapeMarker ) ;
    return static_cast<const StockMarkerShapeData *>(data_.get())->pen_ ;
}

StockMarkerShape Marker::shape() const {
    assert(data_ && type_ == StockShapeMarker ) ;
    return static_cast<const StockMarkerShapeData *>(data_.get())->shape_ ;
}

/*
ShapeDrawable Marker::drawable() const {
    assert(data_ && type_ == CustomShapeMarker ) ;
    return static_cast<const CustomShapeMarkerData *>(data_.get())->drawable_ ;
}
*/

Marker Marker::read(const msg::Marker &marker)
{
    if ( marker.has_stock_shape() ) {
        const msg::StockShape &shape = marker.stock_shape();
        float sz = marker.size() ;
        Pen pen  ;
        Brush brush ;

        if ( shape.has_pen() )
            pen = Pen::read(shape.pen()) ;
        if ( shape.has_brush() )
            brush = Brush::read(shape.brush()) ;

        StockMarkerShape geom ;
        switch ( shape.geometry() ) {
        case msg::StockShape_Geometry_CIRCLE_MARKER:
            geom = StockMarkerShape::Circle ;
            break ;
        case msg::StockShape_Geometry_STAR_MARKER:
            geom = StockMarkerShape::Star ;
            break ;
        case msg::StockShape_Geometry_POINT_MARKER:
            geom = StockMarkerShape::Point ;
            break ;
        case msg::StockShape_Geometry_SQUARE_MARKER:
            geom = StockMarkerShape::Square ;
            break ;
        case msg::StockShape_Geometry_DIAMOND_MARKER:
            geom = StockMarkerShape::Diamond ;
            break ;
        case msg::StockShape_Geometry_X_MARKER:
            geom = StockMarkerShape::XMark ;
            break ;
        case msg::StockShape_Geometry_PLUS_MARKER:
            geom = StockMarkerShape::Plus ;
            break ;
        case msg::StockShape_Geometry_TRIANGLE_UP_MARKER:
            geom = StockMarkerShape::TriangleUp ;
            break ;
        case msg::StockShape_Geometry_TRIANGLE_DOWN_MARKER:
            geom = StockMarkerShape::TriangleDown ;
            break ;
        case msg::StockShape_Geometry_TRIANGLE_LEFT_MARKER:
            geom = StockMarkerShape::TriangleLeft ;
            break ;
        case msg::StockShape_Geometry_TRIANGLE_RIGHT_MARKER:
            geom = StockMarkerShape::TriangleRight ;
            break ;
        }
        return Marker(geom, pen, brush, sz);
    } else {
        return Marker() ;
    }

}

msg::Marker *Marker::write(const Marker &marker)
{
    msg::Marker *msg_marker = new msg::Marker() ;

    msg_marker->set_size(marker.size()) ;

    if ( marker.type() == MarkerType::StockShapeMarker ) {
        msg::StockShape *stock_shape = new msg::StockShape() ;
        Brush brush = marker.brush() ;
        stock_shape->set_allocated_brush(Brush::write(brush));
        stock_shape->set_allocated_pen(Pen::write(marker.pen()));
        switch ( marker.shape() ) {
        case StockMarkerShape::Star:
            stock_shape->set_geometry(msg::StockShape_Geometry_STAR_MARKER);
            break ;
        case StockMarkerShape::Point:
            stock_shape->set_geometry(msg::StockShape_Geometry_POINT_MARKER);
            break ;
        case StockMarkerShape::Circle:
            stock_shape->set_geometry(msg::StockShape_Geometry_CIRCLE_MARKER);
            break ;
        case StockMarkerShape::Square:
            stock_shape->set_geometry(msg::StockShape_Geometry_SQUARE_MARKER);
            break ;
        case StockMarkerShape::Diamond:
            stock_shape->set_geometry(msg::StockShape_Geometry_DIAMOND_MARKER);
            break ;
        case StockMarkerShape::Plus:
            stock_shape->set_geometry(msg::StockShape_Geometry_PLUS_MARKER);
            break ;
        case StockMarkerShape::XMark:
            stock_shape->set_geometry(msg::StockShape_Geometry_X_MARKER);
            break ;
        case StockMarkerShape::TriangleLeft:
            stock_shape->set_geometry(msg::StockShape_Geometry_TRIANGLE_LEFT_MARKER);
            break ;
        case StockMarkerShape::TriangleRight:
            stock_shape->set_geometry(msg::StockShape_Geometry_TRIANGLE_RIGHT_MARKER);
            break ;
        case StockMarkerShape::TriangleUp:
            stock_shape->set_geometry(msg::StockShape_Geometry_TRIANGLE_UP_MARKER);
            break ;
        case StockMarkerShape::TriangleDown:
            stock_shape->set_geometry(msg::StockShape_Geometry_TRIANGLE_DOWN_MARKER);
            break ;
        }

        msg_marker->set_allocated_stock_shape(stock_shape) ;

    }

    return msg_marker ;
}




}
