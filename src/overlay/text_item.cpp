#include "text_item.hpp"
#include "glyph_cache.hpp"
#include "common/shader.hpp"
#include "text_engine.hpp"

using namespace std ;
using namespace Eigen ;

#define DEBUG 0

namespace xviz { namespace impl {

OpenGLText::OpenGLText(const string &text, const Font &font, const Vector3f &clr):
    clr_(clr) {
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    create(text, font) ;
#if DEBUG
    glGenVertexArrays(1, &dec_vao_);
    glBindVertexArray(dec_vao_);

    glGenBuffers(1, &dec_vbo_);
    glGenBuffers(1, &dec_ebo_);

    createDebug() ;
#endif

}


/// Source of the vertex shader used to scale the glyphs vertices
static const char* text_vertex_shader_code = R"(
                                             #version 330
                                             layout(location = 0) in vec2 position;
                                             layout(location = 1) in vec2 texCoord;

                                             smooth out vec2 smoothTexCoord;
                                             uniform mat4 projection;

                                             uniform float aspect ;
                                             uniform vec2 scale;
                                             uniform vec2 offset;

                                             void main() {
                                             gl_Position =  vec4((position + offset)*scale - vec2(1, 1), 0.0f, 1.0f);
                                             smoothTexCoord = texCoord;
                                             }
                                             )";


/// Source of the fragment shader used to draw the glyphs using the cache texture
static const char* text_fragment_shader_code = R"(
                                               #version 330
                                               smooth in vec2 smoothTexCoord;
                                               out vec4 outputColor;

                                               uniform sampler2D textureCache;
                                               uniform vec3 color;

                                               void main() {
                                               float textureIntensity = texture(textureCache, smoothTexCoord).r;
                                               outputColor = vec4(color, textureIntensity);
                                               }
                                               )";


static OpenGLShaderProgram::Ptr get_text_program() {

    static OpenGLShaderProgram::Ptr prog ;

    if ( prog ) return prog ;

    OpenGLShaderPtr vs(new OpenGLShader(VERTEX_SHADER)) ;
    vs->setSourceCode(text_vertex_shader_code);

    OpenGLShaderPtr fs(new OpenGLShader(FRAGMENT_SHADER))  ;
    fs->setSourceCode(text_fragment_shader_code);

    prog.reset(new OpenGLShaderProgram) ;
    prog->addShader(vs) ;
    prog->addShader(fs) ;

    prog->link() ;

    return prog ;
}
#if DEBUG
static const char* dec_vertex_shader_code = R"(
                                            #version 330
                                            layout(location = 0) in vec2 position;

                                            uniform vec2 scale;
                                            uniform vec2 offset;

                                            void main() {
                                            gl_Position =  vec4((position + offset)*scale - vec2(1, 1), 0.0f, 1.0f);
                                            }
                                            )";

static const char* dec_fragment_shader_code = R"(
                                              #version 330

                                              out vec4 outputColor;
                                              ;
                                              uniform vec3 color;

                                              void main() {

                                              outputColor = vec4(color, 1.0);
                                              }
                                              )";

static OpenGLShaderProgram::Ptr get_decoration_program() {

    static OpenGLShaderProgram::Ptr prog ;

    if ( prog ) return prog ;

    OpenGLShaderPtr vs(new OpenGLShader(VERTEX_SHADER)) ;
    vs->setSourceCode(dec_vertex_shader_code);

    OpenGLShaderPtr fs(new OpenGLShader(FRAGMENT_SHADER))  ;
    fs->setSourceCode(dec_fragment_shader_code);

    prog.reset(new OpenGLShaderProgram) ;
    prog->addShader(vs) ;
    prog->addShader(fs) ;

    prog->link() ;

    return prog ;
}
#endif

void OpenGLText::render(float x, float y) {

    glEnable(GL_BLEND);
    // glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

    glDisable(GL_DEPTH_TEST) ;
    glDisable(GL_CULL_FACE) ;

    OpenGLShaderProgram::Ptr prog = get_text_program() ;

    prog->use() ;

    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    GLint w = viewport[2], h = viewport[3] ;

    y = h - y ;

    float aspect = w/static_cast<float>(h);

    prog->setUniform("offset", Vector2f(x, y)) ;
    prog->setUniform("aspect", aspect) ;
    prog->setUniform("scale", Vector2f(2.0f/w, 2.0f/h)) ;
    prog->setUniform("color", clr_) ;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, data_.texture_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Bind to sampler name zero == the currently bound texture's sampler state becomes active (no dedicated sampler)
    glBindSampler(0, 0);

    // Draw the rendered text
    glBindVertexArray(vao_);

    glDrawElements(GL_TRIANGLES, data_.indices_.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0) ;
#if DEBUG

        OpenGLShaderProgram::Ptr debug_prog = get_decoration_program() ;

        debug_prog->use() ;

        debug_prog->setUniform("offset", Vector2f(x, y)) ;
        debug_prog->setUniform("scale", Vector2f(2.0f/w, 2.0f/h)) ;
        debug_prog->setUniform("color", clr_) ;

        // Draw the rendered text
        glBindVertexArray(dec_vao_);

      //  glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);

        glDrawArrays(GL_LINES, 0, 8) ;

        glBindVertexArray(0) ;

#endif
}

void OpenGLText::create(const string &text, const Font &font) {
    TextEngine &te = TextEngine::instance() ;
    std::tie(data_, line_) = te.layout(text, font) ;

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,   data_.vertices_.size() * sizeof(GlyphVertex), &data_.vertices_[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(VERTEX_ATTRIBUTE);
    glVertexAttribPointer(VERTEX_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), nullptr);
    glEnableVertexAttribArray(UV_ATTRIBUTE);
    glVertexAttribPointer(UV_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), reinterpret_cast<GLvoid*>(sizeof(GlyphVertex)/2));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_.indices_.size() * sizeof(GLuint),  &data_.indices_[0], GL_STATIC_DRAW);

    glBindVertexArray(0) ;

}


static std::vector<float> rectangle(float x, float y, float w, float h) {
    std::vector<float> data ;
    data.push_back(x) ; data.push_back(y) ; data.push_back(x + w) ; data.push_back(y) ;
    data.push_back(x+w) ; data.push_back(y) ; data.push_back(x + w) ; data.push_back(y + h) ;
    data.push_back(x + w) ; data.push_back(y + h) ; data.push_back(x) ; data.push_back(y + h) ;
    data.push_back(x) ; data.push_back(y + h) ; data.push_back(x) ; data.push_back(y) ;
    return data ;
}
void OpenGLText::createDebug() {
#if DEBUG

   glBindVertexArray(dec_vao_);
   glBindBuffer(GL_ARRAY_BUFFER, dec_vbo_);

   auto vertices = rectangle(line_.origin(), 0, line_.width(), line_.ascent()) ;

    GLuint indices[] = {0, 1};
    glBufferData(GL_ARRAY_BUFFER,   vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(VERTEX_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void *)0);
    glEnableVertexAttribArray(VERTEX_ATTRIBUTE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dec_ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),  indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
#endif
}


OpenGLText::~OpenGLText() {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
#if DEBUG
    glDeleteVertexArrays(1, &dec_vao_);
    glDeleteBuffers(1, &dec_vbo_);
    glDeleteBuffers(1, &dec_ebo_);
#endif
}

}}
