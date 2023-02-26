#include "gl_image.hpp"
#include "common/shader.hpp"
#include "stb_image.h"

using namespace Eigen ;

namespace xviz { namespace impl {


OpenGLImage::~OpenGLImage() {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
    glDeleteTextures(1, &texture_id_) ;
}

OpenGLImage::OpenGLImage(const Image &image) {
   // stbi_set_flip_vertically_on_load(true);

    unsigned char *data = nullptr ;
    int channels;
    bool loaded = false ;

    if ( image.type() == ImageType::Uri ) {
        std::string path = image.uri() ;
        data = stbi_load(path.c_str(), &width_, &height_, &channels, 0);
        if ( data ) loaded = true ;
    } else if ( image.type() == ImageType::Raw ) {
        if ( image.format() == ImageFormat::encoded ) {
            data = stbi_load_from_memory(image.data(), image.width(), &width_, &height_, &channels, 0);
            if ( data ) loaded = true ;
        }
    }

    if ( !data ) return ;

    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_) ;

    if ( channels == 3 )
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else if ( channels == 4 )
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

    if ( loaded ) stbi_image_free(data);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
}


static const char* vertex_shader_code = R"(
#version 330
layout(location = 0) in vec4 vertex;
out vec2 vUV;

uniform vec2 scale;
uniform vec2 offset;

void main() {
    gl_Position =  vec4((vertex.xy + offset)*scale - vec2(1, 1), 0.0f, 1.0f);
    vUV = vertex.zw;
}
)";

static const char* fragment_shader_code = R"(
#version 330

in vec2 vUV;
uniform sampler2D textureMap;

void main()
{
   gl_FragColor = texture(textureMap, vUV);
 //       gl_FragColor = vec4(vUV.s, vUV.t, 1, 1);
}
)";
static OpenGLShaderProgram::Ptr get_program() {

    static OpenGLShaderProgram::Ptr prog ;

    if ( prog ) return prog ;

    OpenGLShaderPtr vs(new OpenGLShader(VERTEX_SHADER)) ;
    vs->setSourceCode(vertex_shader_code);

    OpenGLShaderPtr fs(new OpenGLShader(FRAGMENT_SHADER))  ;
    fs->setSourceCode(fragment_shader_code);

    prog.reset(new OpenGLShaderProgram) ;
    prog->addShader(vs) ;
    prog->addShader(fs) ;

    prog->link() ;

    return prog ;
}

void OpenGLImage::draw(float x, float y, float w, float h)
{
    glEnable(GL_BLEND);
    // glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

    glDisable(GL_DEPTH_TEST) ;
    glDisable(GL_CULL_FACE) ;
    glEnable(GL_TEXTURE_2D) ;

    float vertices[] = { 0, 0, 0, 0,
                         w, 0, 1, 0,
                         w, -h, 1, 1,
                         0, -h, 0, 1 };

    GLuint idx[] = { 0, 1, 2, 0, 2, 3 } ;

    glBindVertexArray(vao_) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,  sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(POS_ATTRIBUTE);
    glVertexAttribPointer(POS_ATTRIBUTE, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx),  idx, GL_STATIC_DRAW);

    glActiveTexture(GL_TEXTURE0) ;
    glBindTexture(GL_TEXTURE_2D, texture_id_) ;

    OpenGLShaderProgram::Ptr prog = get_program() ;

    prog->use() ;

    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    GLint vw = viewport[2], vh = viewport[3] ;

    prog->setUniform("offset", Vector2f(x, vh - y)) ;
    prog->setUniform("scale", Vector2f(2.0f/vw, 2.0f/vh)) ;
    prog->setUniform("textureMap", 0) ;

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0) ;
}


}}
