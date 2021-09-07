#include <clsim/renderer/gl/shader.hpp>

#include <cstring>
#include <fstream>

#include "gl/gl3w.h"

using namespace std ;
using namespace Eigen ;

namespace clsim { namespace impl {

static void throw_error(const char *error_str, const char *error_desc) {
    ostringstream msg ;
    msg << error_str << error_desc ;
    throw OpenGLShaderError(msg.str()) ;
}

void OpenGLShader::create(OpenGLShaderType t) {
    type_ = t ;

    GLenum shader_type ;
    switch ( type_ ) {
    case VERTEX_SHADER:
        shader_type = GL_VERTEX_SHADER ;
        break ;
    case FRAGMENT_SHADER:
        shader_type = GL_FRAGMENT_SHADER ;
        break ;
    case GEOMETRY_SHADER:
        shader_type = GL_GEOMETRY_SHADER ;
        break ;
    case COMPUTE_SHADER:
        shader_type = GL_COMPUTE_SHADER ;
        break ;
    case TESS_CONTROL_SHADER:
        shader_type = GL_TESS_CONTROL_SHADER ;
        break ;
    case TESS_EVALUATION_SHADER:
        shader_type = GL_TESS_EVALUATION_SHADER ;
    }

    if ( ( handle_ = glCreateShader(GLenum(shader_type)) ) == 0 )
        throw_error("cannot create shader", "") ;
}

OpenGLShader::OpenGLShader(OpenGLShaderType t, const std::string &code, const string &rname) {
    create(t) ;
    addSourceString(code, rname) ;
}

void OpenGLShader::setHeader(const string &header) {
    header_ = header ;
}

void OpenGLShader::addPreProcDefinition(const string &key, const string &val) {
    preproc_ += "#define " + key ;
    if ( !val.empty() ) preproc_ += val ;
    preproc_ += '\n' ;
}

void OpenGLShader::addSourceString(const std::string &code, const string &resource_name) {
    sources_.emplace_back(sources_.size(), resource_name, code) ;
}

void OpenGLShader::compile() {

    if ( compiled_ ) return ;

    std::unique_ptr<const GLchar* [] > sources(new const GLchar * [sources_.size() + 2]);
    std::unique_ptr< GLint []> lengths(new GLint [sources_.size() + 2]) ;

    sources[0] = header_.c_str() ;
    sources[1] = preproc_.c_str() ;

    for ( uint i=0 ; i<sources_.size() ; i++ ) {
        const Source &src = sources_[i] ;
        sources[i+2] = src.data_.c_str() ;
        lengths[i+2] = src.data_.length() ;
    }

    glShaderSource(handle_, sources_.size()+2, sources.get(), lengths.get());
    glCompileShader(handle_);

    GLint success;
    glGetShaderiv(handle_, GL_COMPILE_STATUS, &success);

    if ( !success ) {
        GLchar info_log[1024];
        glGetShaderInfoLog(handle_, 1024, NULL, info_log);

        throw_error("Error compilining shader: ", info_log) ;
    }

    compiled_ = true ;
    sources_.clear() ;
}

void OpenGLShader::addSourceFile(const std::string &fname, const string &resource_name) {

    std::ifstream is(fname);
    std::string contents((std::istreambuf_iterator<char>(is)),
                     std::istreambuf_iterator<char>());

    if ( contents.empty() )
        throw_error("Error reading shader file: ", fname.c_str()) ;

    if ( resource_name.empty() )
        addSourceString(contents, fname) ;
    else
        addSourceString(contents, resource_name) ;
}

OpenGLShader::~OpenGLShader() {
    glDeleteShader(handle_) ;
}

OpenGLShaderProgram::OpenGLShaderProgram(const char *vshader_code, const char *fshader_code) {
    handle_ = glCreateProgram();

    OpenGLShaderPtr vertex_shader = make_shared<OpenGLShader>(VERTEX_SHADER, vshader_code) ;
    OpenGLShaderPtr fragment_shader = make_shared<OpenGLShader>(FRAGMENT_SHADER, fshader_code) ;

    addShader(vertex_shader) ;
    addShader(fragment_shader) ;

    link();
}

void OpenGLShaderProgram::setUniform(const string &name, float v) {
    GLint loc = glGetUniformLocation(handle_, name.c_str()) ;
    if ( loc != -1 ) glUniform1f(loc, v) ;
}

void OpenGLShaderProgram::setUniform(const string &name, GLuint v)
{
    GLint loc = glGetUniformLocation(handle_, name.c_str()) ;
    if ( loc != -1 ) glUniform1ui(loc, v) ;
}

void OpenGLShaderProgram::setUniform(const string &name, GLint v)
{
    GLint loc = glGetUniformLocation(handle_, name.c_str()) ;
    if ( loc != -1 ) glUniform1i(loc, v) ;
}

void OpenGLShaderProgram::setUniform(const string &name, const Vector3f &v)
{
    GLint loc = glGetUniformLocation(handle_, name.c_str()) ;
    if ( loc != -1 ) glUniform3fv(loc, 1, v.data()) ;
}

void OpenGLShaderProgram::setUniform(const string &name, const Vector2f &v)
{
    GLint loc = glGetUniformLocation(handle_, name.c_str()) ;
    if ( loc != -1 ) glUniform2fv(loc, 1, v.data()) ;
}

void OpenGLShaderProgram::setUniform(const string &name, const Vector4f &v)
{
    GLint loc = glGetUniformLocation(handle_, name.c_str()) ;
    if ( loc != -1 ) glUniform4fv(loc, 1, v.data()) ;
}

void OpenGLShaderProgram::setUniform(const string &name, const Matrix3f &v)
{
    GLint loc = glGetUniformLocation(handle_, name.c_str()) ;
    if ( loc != -1 ) glUniformMatrix3fv(loc, 1, GL_FALSE, v.data()) ;
}

void OpenGLShaderProgram::setUniform(const string &name, const Matrix4f &v)
{
    GLint loc = glGetUniformLocation(handle_, name.c_str()) ;
    if ( loc != -1 ) glUniformMatrix4fv(loc, 1, GL_FALSE, v.data()) ;
}

OpenGLShaderProgram::~OpenGLShaderProgram() {
    glDeleteProgram(handle_) ;
}

OpenGLShaderProgram::OpenGLShaderProgram() {
    handle_ = glCreateProgram() ;
}

void OpenGLShaderProgram::addShader(const OpenGLShaderPtr &shader) {
    shader->compile() ;
    shaders_.push_back(shader) ;
}

void OpenGLShaderProgram::addShaderFromString(OpenGLShaderType t, const string &code, const string &resource_name) {
    auto shader = std::make_shared<OpenGLShader>(t) ;
    shader->addSourceString(code, resource_name) ;
    shader->compile() ;
    addShader(shader) ;
}

void OpenGLShaderProgram::addShaderFromFile(OpenGLShaderType t, const string &fname, const string &resource_name) {
    auto shader = std::make_shared<OpenGLShader>(t) ;
    shader->addSourceFile(fname, resource_name) ;
    shader->compile() ;
    addShader(shader) ;
}

void OpenGLShaderProgram::link(bool validate) {

#if 0
    const GLchar* feedbackVaryings[] = { "gl_Position" };
    glTransformFeedbackVaryings(handle_, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
#endif

    for( const auto &shader: shaders_ )
        glAttachShader(handle_, shader->handle()) ;

    GLchar error_log[1024] = { 0 };

    glLinkProgram(handle_) ;

    GLint success;
    glGetProgramiv(handle_, GL_LINK_STATUS, &success);

    if ( success == 0 ) {
        glGetProgramInfoLog(handle_, sizeof(error_log), NULL, error_log);
        throw_error("Error linking shader program", error_log) ;
    }

    if ( validate ) {
        glValidateProgram(handle_);
        glGetProgramiv(handle_, GL_VALIDATE_STATUS, &success);

        if ( !success ) {
            glGetProgramInfoLog(handle_, sizeof(error_log), NULL, error_log);
            throw_error("Invalid shader program", error_log);
        }
    }
}

void OpenGLShaderProgram::use() {
   glUseProgram(handle_) ;
}

OpenGLShaderType type_from_string(const string &s) {
    if ( s == "vertex" ) return VERTEX_SHADER ;
    else if ( s == "fragment" ) return FRAGMENT_SHADER ;
    else if ( s == "geometry" ) return GEOMETRY_SHADER ;
    else if ( s == "compute" ) return COMPUTE_SHADER ;
    else if ( s == "tess_control" ) return TESS_CONTROL_SHADER ;
    else if ( s == "tess_evaluation" ) return TESS_EVALUATION_SHADER ;
    else return VERTEX_SHADER ;
}


OpenGLShaderError::OpenGLShaderError(const string &msg): std::runtime_error(msg) {}

} // internal
} // clsim
