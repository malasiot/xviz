#ifndef XVIZ_TEXT_LAYOUT_ENGINE_HPP
#define XVIZ_TEXT_LAYOUT_ENGINE_HPP

#include <xviz/common/font.hpp>
#include "text_layout.hpp"

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <unicode/unistr.h>
#include <unicode/uscript.h>
#include <unicode/ubidi.h>

#include "scrptrun.h"

#include <string>
#include <map>

namespace xviz {

class Glyph ;
class GlyphRun ;

class TextLayoutEngine {
public:
    TextLayoutEngine() ;

    void setFont(const Font &font) ;
    void setWrapWidth(double w) ;
    void setTextDirection(TextDirection dir) { bidi_mode_ = dir ; }
    void setLineSpacing(double ls) { ls_ = ls ; }

    bool run(const std::string &text, FT_Face) ;

    const Font &getFont() const { return font_desc_ ; }
    double getLineSpacing() const { return ls_ ; }
    double getWrapWidth() const { return wrap_width_ ; }
    TextDirection getTextDirection() const { return bidi_mode_ ; }

    const std::vector<GlyphRun> &lines() const { return lines_ ; }

    double width() const { return width_ ; }
    double height() const { return height_ ; }

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

    bool itemize(icu::UnicodeString &, int32_t start, int32_t end, std::vector<TextItem> &items) ;
    bool itemizeBiDi(icu::UnicodeString &, std::vector<DirectionRun> &d_runs, int32_t start, int32_t end) ;
    bool itemizeScript(icu::UnicodeString &, std::vector<LangScriptRun> &runs) ;
    void mergeRuns(const std::vector<LangScriptRun> &script_runs, const std::vector<DirectionRun> &dir_runs, std::vector<TextItem> &items) ;
    void breakLine(icu::UnicodeString &, int32_t start, int32_t end, FT_Face) ;
    bool shape(icu::UnicodeString &us, GlyphRun &line, FT_Face face) ;

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
    void fillGlyphInfo(GlyphCollection &glyphs, GlyphRun &line) ;
    void clearWidths(int32_t start, int32_t end) ;
    void addLine(GlyphRun&& line) ;
 //   void makeCairoGlyphsAndMetrics(GlyphRun &line);
    void computeHeight();

private:

    Font font_desc_{"Arial", 12} ;
    std::map<unsigned,double> width_map_ ;
    double wrap_width_ = -1 ;

    std::vector<GlyphRun> lines_ ;
    double width_ = 0, height_ = 0, ls_ = 0;
    unsigned glyphs_count_ ;
    char wrap_char_ = ' ';
    bool wrap_before_ = true ;
    bool repeat_wrap_char_ = false;
    TextDirection bidi_mode_ = TextDirection::Auto;


} ;

}

#endif
