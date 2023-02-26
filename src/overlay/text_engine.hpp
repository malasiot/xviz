#pragma once

#include "text_layout_engine.hpp"
#include "glyph_cache.hpp"

namespace xviz { namespace impl {

struct OpenGLTextData {
    GLuint texture_ ;
    std::vector<GlyphVertex> vertices_ ;
    std::vector<GLuint> indices_ ;
};

class TextEngine {
public:
    TextEngine() = default ;

    static TextEngine &instance() {
        static TextEngine text_engine_ ;
        return text_engine_ ;
    }

    std::pair<OpenGLTextData, GlyphRun> layout(const std::string &text, const Font &font,
                          TextDirection dir = TextDirection::Auto) ;

private:

    GlyphAtlasCache glyph_atlas_cache_ ;
    TextLayoutEngine layout_engine_ ;

};

}}
