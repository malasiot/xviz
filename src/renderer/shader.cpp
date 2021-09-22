#include "shader.hpp"

#include <cstring>
#include <fstream>
#include <regex>
#include <iostream>

#include "gl/gl3w.h"

#include "resource_manager.hpp"

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



void OpenGLShader::setHeader(const string &header) {
    header_ = header ;
}

void OpenGLShader::addPreProcDefinition(const string &key, const string &val) {
    preproc_ += "#define " + key ;
    if ( !val.empty() ) preproc_ += val ;
    preproc_ += '\n' ;
}

void OpenGLShader::setSourceCode(const std::string &code) {
    code_ = code ;
    compile() ;
}


void OpenGLShader::preproc(const std::string &file_name, const OpenGLShaderPreproc &defines, bool version_parsed) {
    string fragment = OpenGLShaderResourceManager::fetch(file_name) ;
    if ( fragment.empty() ) throw_error("Cannot load shader resource string from: ", file_name.c_str()) ;

    std::stringstream is(fragment);

    static const std::regex rx("[ \t]*#include[ \t]+<([@\\w\\d_./]+)>");
    std::smatch match;

    while ( is ) {
        string line ;
        std::getline(is, line,'\n') ;

        // append any definition after first #version is encountered
        if ( !version_parsed && line.find("#version") != string::npos ) {

            code_.append(std::move(line)) ;
            code_.append("\n") ;
            for( const auto &dp: defines.defines_ ) {
                code_.append("#define ") ;
                code_.append(dp) ;
                code_.append("\n") ;
            }

            for( const auto &dp: defines.constants_ ) {
                code_.append("#define ") ;
                code_.append(dp.first) ;
                code_.append(" ") ;
                code_.append(dp.second) ;
                code_.append("\n") ;
            }
            version_parsed = true ;
        } else if ( std::regex_match(line, match, rx) ) {
            if ( match.size() == 2 ) {
                std::ssub_match sub_match = match[1];
                std::string include_str = sub_match.str();
                preproc(include_str, defines, version_parsed) ;
            }
        } else {
            code_.append(line) ;
            code_.append("\n") ;
        }

    }
}

void OpenGLShader::setSourceFile(const std::string &file_name, const OpenGLShaderPreproc &defines) {
    resource_name_ = file_name ;

    preproc(file_name, defines, false) ;

    cout << code_ << endl ;
    compile() ;
}


void OpenGLShader::compile() {

    if ( compiled_ ) return ;

    GLint len  ;
    const GLchar *source[1] ;

    source[0] = code_.c_str() ;
    len = code_.length() ;

    glShaderSource(handle_, 1, source, &len);
    glCompileShader(handle_);

    GLint success;
    glGetShaderiv(handle_, GL_COMPILE_STATUS, &success);

    if ( !success ) {
        GLchar info_log[1024];
        glGetShaderInfoLog(handle_, 1024, NULL, info_log);

        throw_error("Error compilining shader: ", info_log) ;
    }

    compiled_ = true ;
}


OpenGLShader::~OpenGLShader() {
    glDeleteShader(handle_) ;
}

OpenGLShaderProgram::OpenGLShaderProgram(const char *vshader_code, const char *fshader_code) {
    handle_ = glCreateProgram();

    OpenGLShaderPtr vertex_shader = make_shared<OpenGLShader>(VERTEX_SHADER) ;
    vertex_shader->setSourceCode(vshader_code) ;
    OpenGLShaderPtr fragment_shader = make_shared<OpenGLShader>(FRAGMENT_SHADER) ;
    fragment_shader->setSourceCode(fshader_code) ;

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

void OpenGLShaderProgram::addShaderFromCode(OpenGLShaderType t, const string &code) {
    auto shader = std::make_shared<OpenGLShader>(t) ;
    shader->setSourceCode(code) ;
    shader->compile() ;
    addShader(shader) ;
}

void OpenGLShaderProgram::addShaderFromFile(OpenGLShaderType t, const string &fname, const OpenGLShaderPreproc &preproc) {
    auto shader = std::make_shared<OpenGLShader>(t) ;
    shader->setSourceFile(fname, preproc) ;
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
