#ifndef XVIZ_TEXT_LAYOUT_HPP
#define XVIZ_TEXT_LAYOUT_HPP

#include <string>
#include <memory>

#include <xviz/common/font.hpp>


class TextLayoutEngine ;

namespace xviz {

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

struct Glyph {

    Glyph(unsigned cp): index_(cp) {}

    unsigned index_;  // glyph code point
    double x_advance_, y_advance_;  // amount to advance cursor
    double x_offset_, y_offset_ ;   // glyphs offset
};

class GlyphRun {

public:

    unsigned numGlyphs() const { return glyphs_.size() ; }

    // line height as given by the font metrics
    double height() const { return height_ ; }

    // line width
    double width() const { return width_ ; }

    // distance from base-line to highest point
    double ascent() const { return ascent_ ; }

    // distance from base-line to lowest hanging point
    double descent() const { return descent_ ;  }

    const std::vector<Glyph> &glyphs() const { return glyphs_ ; }

protected:

    friend class TextLayoutEngine ;

    GlyphRun(int32_t first, int32_t last): first_(first), last_(last) {}

    void addGlyph(Glyph && glyph)  {
        double advance = glyph.x_advance_ ;

        width_ += advance ;
        glyphs_.emplace_back(std::move(glyph));
    }

    double height_ = 0 ; // line height
    double width_ = 0; // line width
    double ascent_ ;  // distance from base-line to highest point
    double descent_ ; // distance from base-line to lowest hanging point

    int32_t first_; // index to first unicode code point in logical order
    int32_t last_;

    std::vector<Glyph> glyphs_ ;
} ;


}

#endif
