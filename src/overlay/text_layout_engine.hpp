#ifndef XVIZ_TEXT_LAYOUT_ENGINE_HPP
#define XVIZ_TEXT_LAYOUT_ENGINE_HPP

#include <xviz/common/font.hpp>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <unicode/unistr.h>
#include <unicode/uscript.h>
#include <unicode/ubidi.h>

#include "scrptrun.h"

#include <string>
#include <map>

namespace xviz { namespace impl {

struct Glyph {

    Glyph(unsigned cp): index_(cp) {}

    unsigned index_;  // glyph code point
    double x_advance_, y_advance_;  // amount to advance cursor
    double x_offset_, y_offset_ ;   // glyphs offset
};

class GlyphRun {

public:

    GlyphRun() = default ;

    unsigned numGlyphs() const { return glyphs_.size() ; }

    // line height as given by the font metrics
    double height() const { return height_ ; }

    // line width
    double width() const { return width_ ; }

    // distance from base-line to highest point
    double ascent() const { return ascent_ ; }

    // distance from base-line to lowest hanging point
    double descent() const { return descent_ ;  }

    double advance() const { return advance_ ; }

    double leading() const { return leading_ ; }

    double origin() const { return origin_ ; }

    const std::vector<Glyph> &glyphs() const { return glyphs_ ; }

protected:

    friend class TextLayoutEngine ;

    GlyphRun(int32_t first, int32_t last): first_(first), last_(last) {}

    void addGlyph(Glyph && glyph)  {
        glyphs_.emplace_back(std::move(glyph));
    }

    double height_ = 0 ; // line height
    double width_ = 0; // line width
    double ascent_ ;  // distance from base-line to highest point
    double descent_ ; // distance from base-line to lowest hanging point
    double advance_ ;
    double leading_ ;
    double origin_ ;

    int32_t first_; // index to first unicode code point in logical order
    int32_t last_;

    std::vector<Glyph> glyphs_ ;
} ;


enum class TextDirection { Auto, LeftToRight, RightToLeft } ;

class TextLayoutEngine {
public:
    TextLayoutEngine() ;

    void setFont(const Font &font) ;
    void setWrapWidth(double w) ;

    GlyphRun run(const std::string &text, FT_Face f, size_t start, size_t stop, TextDirection dir = TextDirection::Auto) ;

    ~TextLayoutEngine();

private:
    struct TextItem {
        uint start_ ;
        uint end_ ;
        hb_script_t script_ ;
        std::string lang_ ;
        hb_direction_t dir_ ;
    } ;

    static hb_direction_t icu_direction_to_hb(UBiDiDirection direction) {
        return (direction == UBIDI_RTL) ? HB_DIRECTION_RTL : HB_DIRECTION_LTR;
    }

    using DirectionRun = std::tuple<hb_direction_t, uint, uint> ;
    using LangScriptRun = std::tuple<hb_script_t, uint, uint> ;

    bool itemize(icu::UnicodeString &, int32_t start, int32_t end, TextDirection dir, std::vector<TextItem> &items) ;
    bool itemizeBiDi(icu::UnicodeString &, std::vector<DirectionRun> &d_runs, int32_t start, int32_t end) ;
    bool itemizeScript(icu::UnicodeString &, std::vector<LangScriptRun> &runs) ;
    void mergeRuns(const std::vector<LangScriptRun> &script_runs, const std::vector<DirectionRun> &dir_runs, std::vector<TextItem> &items) ;
    bool shape(icu::UnicodeString &us, GlyphRun &line, FT_Face face, TextDirection dir) ;

    struct GlyphInfo {
        hb_glyph_info_t glyph_;
        hb_glyph_position_t position_;
    };

    struct GlyphCollection {
        unsigned num_glyphs_ ;
        std::vector<std::vector<GlyphInfo>> glyphs_ ;
        std::vector<unsigned> clusters_ ;
    } ;

   bool getGlyphsAndClusters(hb_buffer_t *buffer, GlyphCollection &glyphs) ;
   void fillGlyphInfo(GlyphCollection &glyphs, hb_font_t *font, GlyphRun &line) ;


} ;

}}

#endif
