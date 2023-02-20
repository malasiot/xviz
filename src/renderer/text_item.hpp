#ifndef XVIZ_TEXT_ITEM_HPP
#define XVIZ_TEXT_ITEM_HPP

#include "font_manager.hpp"
#include "glyph_cache.hpp"

#include <Eigen/Core>

namespace xviz { namespace impl {

class TextItem {
public:
    TextItem(const std::string &text, const Font &f) ;
    ~TextItem() ;

    void render(float x, float y, const Eigen::Vector3f &clr) ;

private:

    GLuint vao_, vbo_, ebo_, texture_ ;
    TextQuads quads_ ;

    static const GLuint VERTEX_ATTRIBUTE = 0 ;
    static const GLuint UV_ATTRIBUTE = 1 ;
    static const GLuint TEXTURE_UNIT = 0 ;

};


}}

#endif
