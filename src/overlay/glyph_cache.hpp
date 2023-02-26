#ifndef XVIZ_GLYPH_CACHE_HPP
#define XVIZ_GLYPH_CACHE_HPP

#include "font_manager.hpp"

#include <array>

#include "common/gl/gl3w.h"
#include <hb-ft.h>

namespace xviz { namespace impl {

class GlyphRun ;
class TextLayoutParams ;

struct GlyphVertex {
    GLfloat x_ ;
    GLfloat y_ ;
    GLfloat u_ ;
    GLfloat v_ ;
};
struct TextQuads {
    std::vector<GlyphVertex> vertices_ ;
    std::vector<GLuint> indices_ ;
};

class GlyphAtlas ;
using GlyphAtlasCache = std::map< std::pair<FT_Face, size_t>, GlyphAtlas> ;

class GlyphAtlas {
public:
    GlyphAtlas(FT_Face face, size_t pixel_size) ;
    ~GlyphAtlas() ;

    // performs shaping and composition of quads that may be used for rendering
    void prepare(const std::string &characters, TextQuads &td) ;

    void prepare(const GlyphRun &res, std::vector<GlyphVertex> &vertices,
                 std::vector<GLuint> &indices) ;


    GLuint textureId() const { return texture_ ; }

private:

     using GlyphQuad = std::array<GlyphVertex, 4> ;

    void cache(hb_codepoint_t cp, GlyphQuad &) ;

    FT_Face face_ ;
    size_t sz_ ;
    hb_font_t *font_ ;

    size_t width_ = 256 ;
    size_t height_ = 256 ;
    size_t y_ = 0, x_ = 0, line_height_ = 0 ;

    GLuint texture_ ;

    std::map<hb_codepoint_t, GlyphQuad> glyph_map_ ;

    static const GLuint TEXTURE_UNIT = 0 ;
    static const uint PADDING = 1 ;

public:
    static GlyphAtlasCache g_glyphs ;
};


}}

#endif
