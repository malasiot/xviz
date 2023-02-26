#include "text_engine.hpp"
#include "glyph_cache.hpp"

using namespace std ;

namespace xviz { namespace impl {

std::pair<OpenGLTextData, GlyphRun> TextEngine::layout(const std::string &text, const Font &font,
                                  TextDirection dir)
{
    FT_Face f = FontManager::instance().queryFontFace(font) ;

    auto it = glyph_atlas_cache_.find(make_pair(f, font.size())) ;
    if ( it ==  glyph_atlas_cache_.end() )
        it =  glyph_atlas_cache_.emplace(std::piecewise_construct,
                             std::forward_as_tuple(f, (size_t)font.size()),
                             std::forward_as_tuple(f, font.size())).first ;

    GlyphRun res = layout_engine_.run(text, f, 0, text.length(), dir) ;

    GlyphAtlas &atlas = it->second ;

    OpenGLTextData data ;
    atlas.prepare(res, data.vertices_, data.indices_) ;
    data.texture_ = atlas.textureId() ;


    return make_pair(data, res) ;
}


}}
