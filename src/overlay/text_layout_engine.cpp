#include "text_layout_engine.hpp"
#include "font_manager.hpp"

#include <memory>
#include <iostream>

#include <unicode/brkiter.h>

#include "scrptrun.h"

using namespace std ;

namespace xviz { namespace impl {

bool TextLayoutEngine::itemizeBiDi(icu::UnicodeString &us, vector<DirectionRun> &d_runs, int32_t s_begin, int32_t s_end) {

    int32_t s_length = s_end - s_begin ;
    // Adapted from https://github.com/asmaAL-Bahanta/icu-BiDi-Example/blob/master/bidiExample.cpp

    std::unique_ptr<UBiDi, decltype(&ubidi_close)> bidi(ubidi_open(), ubidi_close) ;
    UErrorCode error_code = U_ZERO_ERROR;
    UBiDiLevel para_level= UBIDI_DEFAULT_LTR;

    // initialize algorithm with string
    ubidi_setPara(bidi.get(), us.getBuffer() + s_begin, s_length, para_level, nullptr, &error_code);

    if ( U_SUCCESS(error_code) ) {
        UBiDiDirection direction = ubidi_getDirection(bidi.get());
        // if the string has a unique direction we are done
        if ( direction != UBIDI_MIXED )
            d_runs.emplace_back(icu_direction_to_hb(direction), s_begin, s_end);
        else {
            // enumerate detected directions
            int32_t count = ubidi_countRuns(bidi.get(), &error_code);

            if ( U_SUCCESS(error_code) ) {
                for( int32_t i=0; i<count; i++ ) {
                    int32_t run_start, run_length;

                    UBiDiDirection dir = ubidi_getVisualRun(bidi.get(), i, &run_start, &run_length);

                    run_start += s_begin ;

                    d_runs.emplace_back(icu_direction_to_hb(dir), run_start, run_start + run_length);
                }
            }
            else return false ;
        }
    }
    else
        return false ;

    return true ;
}



bool TextLayoutEngine::itemizeScript(icu::UnicodeString &us, vector<LangScriptRun> &runs) {

    ScriptRun script_run(us.getBuffer(), us.length());

    while ( script_run.next() ) {
        int32_t run_start = script_run.getScriptStart();
        int32_t run_end = script_run.getScriptEnd();
        UScriptCode run_code = script_run.getScriptCode();

        hb_script_t hb_script ;
        if ( run_code == USCRIPT_INVALID_CODE)
            hb_script = HB_SCRIPT_INVALID;
        else
            hb_script = hb_script_from_string(uscript_getShortName(run_code), -1);

        runs.emplace_back(hb_script, run_start, run_end);
    }

    return true ;
}


void TextLayoutEngine::mergeRuns(const vector<LangScriptRun> &script_runs, const vector<DirectionRun> &dir_runs, vector<TextItem> &items)
{
    for (auto &dir_run : dir_runs)
    {
        uint start = std::get<1>(dir_run) ;
        uint end = std::get<2>(dir_run);

        auto rtl_insertion_point = items.end();

        auto ms_it = script_runs.end() ;
        for ( auto it = script_runs.begin(); it != script_runs.end(); ++it ) {
            if (( std::get<1>(*it) <= start ) && ( std::get<2>(*it) > start) ) {
                ms_it = it ;
                break ;
            }
        }

        while (start < end)
        {
            TextItem item;
            item.start_ = start ;
            item.end_ = std::min(std::get<2>(*ms_it), end);
            item.script_ = std::get<0>(*ms_it) ;
            item.lang_ = ScriptRun::detectLanguage(item.script_);
            item.dir_ = std::get<0>(dir_run) ;

            if ( item.dir_ == HB_DIRECTION_LTR )
                items.emplace_back(item);
            else
                rtl_insertion_point = items.insert(rtl_insertion_point, item);

            start = item.end_ ;

            if ( std::get<2>(*ms_it) == start)
                ++ms_it;
        }
    }
}


bool TextLayoutEngine::getGlyphsAndClusters(hb_buffer_t *buffer,  GlyphCollection &glyphs) {

    unsigned num_glyphs = hb_buffer_get_length(buffer);
    if ( num_glyphs == 0 ) return false ;

    glyphs.num_glyphs_ = num_glyphs ;

    hb_glyph_info_t *hb_glyphs = hb_buffer_get_glyph_infos(buffer, &num_glyphs);
    hb_glyph_position_t *hb_positions = hb_buffer_get_glyph_positions(buffer, &num_glyphs);

    unsigned cluster = hb_glyphs[0].cluster ;
    bool in_cluster = false;

    // collect clusters and associated glyphs

    glyphs.clusters_.push_back(cluster) ;

    for ( unsigned i = 0; i < num_glyphs; ++i ) {
        if (cluster != hb_glyphs[i].cluster) {
            cluster = hb_glyphs[i].cluster;
            glyphs.clusters_.push_back(cluster) ;
            in_cluster = false;
        }
        else in_cluster = true ;

        if ( glyphs.glyphs_.size() <= cluster )
            glyphs.glyphs_.resize(cluster + 1);

        auto &c = glyphs.glyphs_[cluster];
        if (c.empty())
            c.push_back({hb_glyphs[i], hb_positions[i]});
        else if (c.front().glyph_.codepoint == 0)
            c.front() = { hb_glyphs[i], hb_positions[i] };
        else if (in_cluster)
            c.push_back({ hb_glyphs[i], hb_positions[i] });
    }

    // check if all clusters/characters have associated glyphs

    bool all_set = true;

    for (const auto &c : glyphs.glyphs_)
    {
        if ( c.empty() || c.front().glyph_.codepoint == 0 ) {
            all_set = false;
            break;
        }
    }

    return all_set ;

}


void TextLayoutEngine::fillGlyphInfo(GlyphCollection &glyphs, hb_font_t *font, GlyphRun &line) {
    // iterate all clusters

    double advance = 0 ;
    float x = 0, minx = 1000, maxx = 0;
    for (auto c_id : glyphs.clusters_)
    {
        // iterate over all glyphs in this cluster

        auto &c = glyphs.glyphs_[c_id] ;

        for ( auto const& info : c ) {

            auto const& gpos = info.position_ ;
            auto const& glyph = info.glyph_ ;

            unsigned char_index = glyph.cluster;
            Glyph g(glyph.codepoint);

            hb_glyph_extents_t extents ;
            hb_font_get_glyph_extents(font, glyph.codepoint, &extents) ;

            float x1 = extents.x_bearing/64.0;
            float y1 = -extents.y_bearing/64.0;
            float width = extents.width/64.0;
            float height = -extents.height/64.0;

            minx = std::min(minx, x + x1) ;
            maxx = std::max(maxx, x + width) ;

            g.x_advance_ = gpos.x_advance/64.0 ;
            g.y_advance_ = gpos.y_advance/64.0 ;
            g.x_offset_ = gpos.x_offset/64.0;
            g.y_offset_ = gpos.y_offset/64.0 ;

            x += g.x_advance_ ;



            line.addGlyph(std::move(g)) ;
            line.ascent_ = std::max(line.ascent_, extents.y_bearing/64.0);
            line.descent_ = std::min(line.descent_, (extents.height + extents.y_bearing)/64.0) ;
            line.height_ = std::max(line.height_, -extents.height/64.0) ;
            line.width_ += g.x_advance_ ;

            advance = g.x_advance_ - extents.width/64.0 + extents.x_bearing/64.0 ;
        }
    }

    line.advance_ = advance ;
   // line.width_ -= advance ;

    hb_font_extents_t extents ;
    hb_font_get_h_extents (font, &extents);
    line.leading_ = (extents.ascender - extents.descender + extents.line_gap)/64.0 ;
    line.width_ = maxx - minx ;
    line.origin_ = minx ;
}


bool TextLayoutEngine::shape(icu::UnicodeString &us, GlyphRun &line, FT_Face face, TextDirection dir)
{
    unsigned start = line.first_ ;
    unsigned end = line.last_ ;
    std::size_t length = end - start;

    if ( !length ) return true ;

    // itemize text span
    vector<TextItem> items ;
    itemize(us, start, end, dir, items);

    // prepare HarfBuzz shaping engine

    line.glyphs_.reserve(length);

    auto hb_buffer_deleter = [](hb_buffer_t * buffer) { hb_buffer_destroy(buffer);};
    const std::unique_ptr<hb_buffer_t, decltype(hb_buffer_deleter)> buffer(hb_buffer_create(), hb_buffer_deleter);

    hb_buffer_pre_allocate(buffer.get(), length);

    // perform shaping for each item, with unique script, direction

    for ( const auto & text_item : items ) {

        GlyphCollection glyphs ;

        // initialize buffer with subtext and corresponding direction and script

        hb_buffer_clear_contents(buffer.get());
        hb_buffer_add_utf16(buffer.get(), reinterpret_cast<const uint16_t *>(us.getBuffer()), us.length(), text_item.start_, static_cast<int>(text_item.end_ - text_item.start_));
        hb_buffer_set_direction(buffer.get(), text_item.dir_);

        if ( !text_item.lang_.empty() )
            hb_buffer_set_language(buffer.get(), hb_language_from_string(text_item.lang_.c_str(), -1));

        hb_buffer_set_script(buffer.get(), text_item.script_);

        //   hb_ft_font_set_load_flags(font,FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING);

        hb_font_t *hb_font = hb_ft_font_create(face, nullptr);

        // run shaper on this segment and font

        hb_shape(hb_font, buffer.get(), 0, 0);


       // get resulting glyphs and find which of the characters were correctly mapped by the current font face

        getGlyphsAndClusters(buffer.get(), glyphs) ;

        fillGlyphInfo(glyphs, hb_font, line);

        hb_font_destroy(hb_font);

    }


    return true ;
}

bool TextLayoutEngine::itemize(icu::UnicodeString &us, int32_t start, int32_t end, TextDirection dir, vector<TextItem> &items) {
    using namespace icu ;

    // itemize directions
    vector<DirectionRun> dir_runs ;
    if ( dir ==TextDirection::Auto ) {
        if ( !itemizeBiDi(us, dir_runs, start, end) ) return false ;
    } else if ( dir == TextDirection::LeftToRight ) {
        dir_runs.emplace_back(HB_DIRECTION_LTR, start, end) ;
    } else {
        dir_runs.emplace_back(HB_DIRECTION_RTL, start, end) ;
    }

    // itemize scripts
    vector<LangScriptRun> script_runs ;
    if ( !itemizeScript(us, script_runs) ) return false ;

    mergeRuns(script_runs, dir_runs, items);

    return true ;
}


TextLayoutEngine::TextLayoutEngine() {

}

TextLayoutEngine::~TextLayoutEngine() {

}


GlyphRun TextLayoutEngine::run(const std::string &text, FT_Face face, size_t start, size_t end, TextDirection dir) {
    icu::UnicodeString us = icu::UnicodeString::fromUTF8(text) ;
    GlyphRun line(start, end);
    shape(us, line, face, dir);
    return line ;
}

}}
