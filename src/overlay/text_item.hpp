#ifndef XVIZ_TEXT_ITEM_HPP
#define XVIZ_TEXT_ITEM_HPP

#include "font_manager.hpp"
#include "glyph_cache.hpp"
#include "text_layout_engine.hpp"
#include "text_engine.hpp"

#include <memory>

#include <Eigen/Core>

namespace xviz { namespace impl {

class OpenGLTextData ;

class OpenGLText {
public:
    OpenGLText(const std::string &text, const Font &f, const Eigen::Vector3f &clr) ;
    ~OpenGLText() ;

    void render(float x, float y) ;

    const GlyphRun &line() const { return line_ ; }

private:

    void create(const std::string &text, const Font &f) ;

    GLuint vao_, vbo_, ebo_ ;
    OpenGLTextData data_ ;
    GlyphRun line_ ;
    Eigen::Vector3f clr_ ;

     GLuint dec_vao_, dec_vbo_, dec_ebo_ ;

    static const GLuint VERTEX_ATTRIBUTE = 0 ;
    static const GLuint UV_ATTRIBUTE = 1 ;
    static const GLuint TEXTURE_UNIT = 0 ;
    void createDebug();

};


}}

#endif
