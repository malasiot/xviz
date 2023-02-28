#include "gl_rect.hpp"

using namespace Eigen ;

namespace xviz { namespace impl {

OpenGLRect::OpenGLRect() {
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
}

OpenGLRect::~OpenGLRect() {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
}

static const char* vertex_shader_code = R"(
                                            #version 330
                                            layout(location = 0) in vec2 position;

                                            uniform vec2 scale;
                                            uniform vec2 offset;

                                            void main() {
                                            gl_Position =  vec4((position + offset)*scale - vec2(1, 1), 0.0f, 1.0f);
                                            }
                                            )";

static const char* fragment_shader_code = R"(
                                              #version 330

                                              out vec4 outputColor;

                                              uniform vec3 color;

                                              void main() {

                                              outputColor = vec4(color, 1.0);
                                              }
                                              )";
static const char* v_fragment_shader_code = R"(
uniform vec2 u_resolution;
uniform vec3 fill;
uniform vec3 stroke;
uniform float thickness;
uniform vec4 coords ;
uniform float opacity ;

vec3 drawRect(in vec2 st, in vec2 center, in float width,
  in float height, in float thickness, in vec3 fillColor,
  in vec3 strokeColor) {

  vec3 color = vec3(0);
  float halfWidth = width * .5;
  float halfHeight = height * .5;
  float halfTickness = thickness * .5;

  vec2 bottomLeft = vec2(center.x - halfWidth, center.y - halfHeight);
  vec2 topRight = vec2(center.x + halfWidth, center.y + halfHeight);

  vec2 stroke = vec2(0.0);
  stroke += step(bottomLeft-halfTickness, st) * (1.0 - step(bottomLeft+halfTickness, st));
  stroke += step(topRight-halfTickness, st) * (1.0 - step(topRight+halfTickness, st));
  vec2 strokeLimit = step(bottomLeft-halfTickness, st) * (1.0 - step(topRight+halfTickness, st));
  stroke *= strokeLimit.x * strokeLimit.y;

  color = mix (color, strokeColor, min(stroke.x + stroke.y, 1.0));

  vec2 fill = vec2(0.0);
  fill += step(bottomLeft+halfTickness, st) * (1.0 - step(topRight-halfTickness, st));
  vec2 fillLimit = step(bottomLeft+halfTickness, st) * (1.0 - step(topRight-halfTickness, st));
  fill *=  fillLimit.x * fillLimit.y;

  color = mix (color, fillColor, min(fill.x + fill.y, 1.0));

  return color;
}

void main(){
    vec2 st = gl_FragCoord.xy;

    vec2 center = coords.xy ;
    float width = coords.z ;
    float height = coords.w ;

    gl_FragColor = vec4(drawRect(st, center, width, height, thickness, fill, stroke), opacity);

}
    )";
static OpenGLShaderProgram::Ptr get_program() {

    static OpenGLShaderProgram::Ptr prog ;

    if ( prog ) return prog ;

    OpenGLShaderPtr vs(new OpenGLShader(VERTEX_SHADER)) ;
    vs->setSourceCode(vertex_shader_code);

    OpenGLShaderPtr fs(new OpenGLShader(FRAGMENT_SHADER))  ;
    fs->setSourceCode(v_fragment_shader_code);

    prog.reset(new OpenGLShaderProgram) ;
    prog->addShader(vs) ;
    prog->addShader(fs) ;

    prog->link() ;

    return prog ;
}

void OpenGLRect::render(float x, float y, float w, float h, const Eigen::Vector3f &fill_clr,
                        const Eigen::Vector3f &border_clr, float border_width, float opacity)
{
    glEnable(GL_BLEND);
    // glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

    glDisable(GL_DEPTH_TEST) ;
    glDisable(GL_CULL_FACE) ;

    float bw = border_width/2 ;
    float pos[] = { -bw, bw, w + bw, bw, w + bw, -h - bw, -bw, -h - bw };
    GLuint idx[] = { 0, 1, 2, 0, 2, 3 } ;

    glBindVertexArray(vao_) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,  sizeof(pos), pos, GL_STATIC_DRAW);
    glEnableVertexAttribArray(POS_ATTRIBUTE);
    glVertexAttribPointer(POS_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 2* sizeof(float), nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx),  idx, GL_STATIC_DRAW);

    OpenGLShaderProgram::Ptr prog = get_program() ;

    prog->use() ;

    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    GLint vw = viewport[2], vh = viewport[3] ;

    prog->setUniform("offset", Vector2f(x, vh - y)) ;
    prog->setUniform("scale", Vector2f(2.0f/vw, 2.0f/vh)) ;
    prog->setUniform("fill", fill_clr) ;
    prog->setUniform("stroke", border_clr) ;
    prog->setUniform("thickness", border_width) ;
    prog->setUniform("opacity", opacity);
    prog->setUniform("coords", Vector4f{x + w/2.0, (vh - y - h/2.0), w, h});
    prog->setUniform("u_resolution", Vector2f(vw, vh)) ;


    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0) ;
}


}}
