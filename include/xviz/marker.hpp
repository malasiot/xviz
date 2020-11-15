#ifndef XVIZ_MARKER_HPP
#define XVIZ_MARKER_HPP

#include <xviz/pen.hpp>
#include <xviz/brush.hpp>
#include <xviz/drawable.hpp>

namespace xviz {

namespace msg {
    class Marker ;
}

enum MarkerType { NoMarker, StockShapeMarker, CustomShapeMarker } ;
enum StockMarkerShape { Point, Square, Circle, Diamond, Plus, XMark, Star, TriangleUp, TriangleDown, TriangleLeft, TriangleRight  } ;

class MarkerData ;

class Marker {
public:
    Marker() ;
    Marker(StockMarkerShape shape, Pen pen, Brush brush, double sz) ;
    Marker(const Drawable &d, double sz) ;

    MarkerType type() const { return type_ ; }
    double size() const ;
    Pen pen() const  ;
    Brush brush() const  ;
    Drawable drawable() const ;
    StockMarkerShape shape() const ;

    static Marker read(const msg::Marker &marker) ;
    static msg::Marker *write(const Marker &marker) ;

protected:

    std::shared_ptr<MarkerData> data_ ;
    MarkerType type_ ;
};

}

#endif
