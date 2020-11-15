#ifndef XVIZ_DRAWABLE_HPP
#define XVIZ_DRAWABLE_HPP

#include <vector>
#include <cassert>

#include <xviz/brush.hpp>
#include <xviz/pen.hpp>
#include <xviz/path.hpp>
#include <xviz/font.hpp>

namespace xviz {

namespace msg {
    class Drawable ;
}

class Drawable {
  public:

    virtual ~Drawable() = default ;

    Drawable &setBrush(Brush b) { brush_ = b ; return *this ; }
    Drawable &setPen(Pen p) { pen_ = p ; return *this ;}
    Drawable &setTransform(const Matrix2d &mat) { transform_ = mat ; return *this; }
    Drawable &setFont(FontHandle f) { font_ = f ; return *this ; }

    Brush brush() const ;
    Pen pen() const ;
    Matrix2d transform() const ;
    Matrix2d globalTransform() const ;

    void setParent(Drawable *d) { parent_ = d ; }

    static msg::Drawable *write(const Drawable *d) ;
    static Drawable *read(const msg::Drawable &msg) ;

protected:
    Brush brush_ ;
    Pen pen_ ;
    Matrix2d transform_ ;
    FontHandle font_ ;
    Drawable *parent_ = nullptr ;
};

using DrawableHandle = std::shared_ptr<Drawable> ;

class Path ;

class ShapeDrawable: public Drawable {
public:
    ShapeDrawable(const Path &path): path_(path) {}

    Path &path() { return path_ ; }
    const Path &path() const { return path_ ; }

protected:

    Path path_ ;
};


enum class TextDirection { Auto, LeftToRight, RightToLeft } ;

enum TextAlignFlags {
    TextAlignLeft = 0x01, TextAlignRight = 0x02, TextAlignTop = 0x04, TextAlignBottom = 0x08,
    TextAlignHCenter = 0x10, TextAlignVCenter = 0x20, TextWordWrap = 0x40
}  ;

enum TextDecoration {
    TextDecorationNone, TextDecorationUnderline, TextDecorationStrikeThrough
};

class TextDrawable: public Drawable {
public:

    TextDrawable(const std::string &text, Rectangle2d &rect, int align = TextAlignLeft|TextAlignTop):
        text_(text), rect_(rect), align_flags_(align) {}

    TextDrawable(const std::string &text, double x, double y):
        text_(text), rect_(Rectangle2d(x, y, 0, 0)) {}

    int textAlignFlags() const { return align_flags_ ; }

    TextDecoration textDecoration() const { return decoration_ ; }
    void setTextDecoration(TextDecoration t) { decoration_ = t ; }

    const std::string &text() const { return text_ ; }
    const Rectangle2d &rect() const { return rect_ ; }

protected:

    std::string text_ ;
    int align_flags_ ;
    TextDecoration decoration_ ;
    Rectangle2d rect_ ;
};

class GroupDrawable: public Drawable {
public:
    void addChild(Drawable *d) {
        assert(d) ;
        d->setParent(this) ;
        children_.push_back(std::move(std::unique_ptr<Drawable>(d))) ;
    }

    const std::vector<std::unique_ptr<Drawable>> &children() const { return children_; }

protected:

    std::vector<std::unique_ptr<Drawable>> children_ ;
};


}

#endif
