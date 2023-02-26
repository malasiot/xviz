#ifndef XVIZ_TEXT_LAYOUT_HPP
#define XVIZ_TEXT_LAYOUT_HPP

#include <string>
#include <memory>

#include <xviz/common/font.hpp>

namespace xviz { namespace impl {

enum class TextDirection { Auto, LeftToRight, RightToLeft } ;

enum TextAlignFlags {
    TextAlignLeft = 0x01, TextAlignRight = 0x02, TextAlignTop = 0x04, TextAlignBottom = 0x08, TextAlignHCenter = 0x10, TextAlignVCenter = 0x20, TextAlignBaseline = 0x40
}  ;

enum TextDecoration {
    TextDecorationNone, TextDecorationUnderline, TextDecorationStrikeThrough
};

struct Rectangle2d {
  float x_, y_, width_, height_ ;
};

class GlyphRun ;
class TextLayoutEngine ;

// The Text object can be used to cache text layout for repeated text drawing
// It can be also used to measure the string bounding box

class Text {
public:
    Text()  ;
    Text(Text &&t) ;
    Text(const std::string &text) ;
    ~Text() ;


public:

    // when drawing directly to canvas i.e. via drawText calls, these are set by the current context

    void setText(const std::string &text) ;
    void setFont(const Font &font) ;
    void setLineSpacing(double ls) ;
    void setWrapWidth(double width) ;
    void setTextDirection(TextDirection dir) ;

    // force layout computation
     void updateLayout() ;

    // get a the text box dimensions ( the font and other layout parameters have to be set first )

    double width() ;
    double height() ;
    double advance() ;

    // get the box containing the text as will be drawn given the provided alignment flags
    Rectangle2d box(const Rectangle2d &box, uint flags) ;

    // get the internal text layout

    const std::vector<GlyphRun> &lines() ;

private:



    bool needs_update_ = true ;

    std::unique_ptr<TextLayoutEngine> engine_ ;
};



}}

#endif
