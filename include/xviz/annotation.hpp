#ifndef XVIZ_ANNOTATION_HPP
#define XVIZ_ANNOTATION_HPP

#include <vector>
#include <cassert>

#include <xviz/brush.hpp>
#include <xviz/pen.hpp>
#include <xviz/path.hpp>
#include <xviz/font.hpp>

namespace xviz {

namespace msg {
    class Annotation ;
}

class AnnotationData ;

class Annotation {
public:
    enum Type { Shape, Label, Marker } ;

    Type type() const ;

    static msg::Annotation *write(const Annotation &d) ;
    static Annotation read(const msg::Annotation &msg) ;

protected:
    std::shared_ptr<AnnotationData> data_ ;
};

class Path ;

// Draw one or more shapes
// The shape is drawn with the given brush and pen

class ShapeAnnotation: public Annotation {
public:
    ShapeAnnotation() ;

    ShapeAnnotation &setBrush(Brush b) ;
    ShapeAnnotation &setPen(Pen p) ;

    Brush brush() const ;
    Pen pen() const ;

    ShapeAnnotation &addShape(const Path &path) ;

    const std::vector<Path> &shapes() const ;
};

// This is used to draw the same shape at multiple locations
// Thus the path should be centered at (0, 0) and scaled to logical coordinates
// The shape is drawn using the containing layer brush and pen

class MarkerAnnotation: public Annotation {
public:
    MarkerAnnotation(const Path &marker) ;

    MarkerAnnotation &addPosition(float x, float y) ;

    Brush brush() const ;
    Pen pen() const ;

    MarkerAnnotation &setBrush(Brush b) ;
    MarkerAnnotation &setPen(Pen p) ;

    Path marker() const ;
    const std::vector<Vector2d> &positions() const ;
};

class LabelAnnotation: public Annotation {
public:

    LabelAnnotation() ;

    enum AlignFlags {
        TextAlignLeft = 0x01, TextAlignRight = 0x02, TextAlignTop = 0x04, TextAlignBottom = 0x08,
        TextAlignHCenter = 0x10, TextAlignVCenter = 0x20, TextWordWrap = 0x40
    }  ;

    LabelAnnotation &setBrush(Brush b) ;
    LabelAnnotation &setPen(Pen p) ;
    LabelAnnotation &setFont(FontHandle f) ;
    LabelAnnotation &setAlignFlags(int align) ;
    LabelAnnotation &setOffset(float x, float y) ;

    Brush brush() const ;
    Pen pen() const ;
    FontHandle font() const ;
    int alignFlags() const ;
    Vector2d offset() const ;

    LabelAnnotation &addLabel(const std::string &text, float x, float y) ;

    const std::vector<Vector2d> &positions() const ;
    const std::vector<std::string> &labels() const ;
};


}

#endif
