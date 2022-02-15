#include "shader.hpp"

#include <cstring>
#include <fstream>
#include <regex>
#include <iostream>
#include <cstdint>


#include "resource_manager.hpp"

using namespace std ;
using namespace Eigen ;

namespace xviz { namespace impl {

static void throw_error(const std::string &code, const char *error_str, const char *error_desc) {
#if 1
    istringstream is(code);
    string str;
    int line = 1 ;
    while(getline(is, str))
    {
        cout << line << ":\t" << str << endl ;
        ++line ;
    }
#endif
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
        throw_error(code_, "cannot create shader", "") ;
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
    if ( fragment.empty() ) throw_error(code_, "Cannot load shader resource string from: ", file_name.c_str()) ;

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
        } else if ( line.find("#pragma unroll_loop_start") != string::npos ) {
            string loop ;
            while ( is ) {
                std::getline(is, line,'\n') ;
                if ( line.find("#pragma unroll_loop_end") != string::npos ) break ;
                else loop += line + '\n';
            }
            unrollLoop(loop, defines.constants_) ;

        } else {
            code_.append(line) ;
            code_.append("\n") ;
        }

    }


}

void OpenGLShader::unrollLoop(const string &loop, const std::map<std::string, std::string> &constants )
{

     static const std::regex for_loop_rx(R"(\s*for\s*\(\s*int\s+i\s*=\s*(\d+)\s*;\s*i\s*<\s*(\w+)\s*;\s*i\s*\+\+\s*\)\s*\{([\S\s]*)$)",
                                         std::regex_constants::ECMAScript);

    std::smatch match;

    if ( !std::regex_match(loop, match, for_loop_rx) ) {
        assert("Unsupported for loop construct") ;
        return ;
    }

    auto loop_start = match[1].str() ;
    auto loop_sz = match[2].str() ;
    auto remaining = match[3].str() ;

    // deal with nested blocks
    string loop_body ;

    int brackets = 1 ;
    for( char c : remaining ) {
        if ( c == '{' ) {
            ++ brackets ;
            loop_body.push_back(c) ;
        } else if ( c == '}' ) {
            --brackets ;
            if ( brackets == 0 ) break ;
            else loop_body.push_back(c) ;
        } else
            loop_body.push_back(c) ;
    }

    // replace loop sz with variable

    for( const auto &cp: constants ) {
        if ( cp.first == loop_sz ) {
            loop_sz = cp.second ;
            break ;
        }
    }


    int loop_sz_num = 0 ;
    try {
        loop_sz_num = stoi(loop_sz) ;
    } catch ( std::exception &e ) {
        // if a variable was not defined ignore loop
    }


    // write the unrolled loop

    static regex index_rx("\\[\\s*i\\s*\\]") ;
    static regex var_rx("UNROLLED_LOOP_INDEX") ;
    for ( int i=stoi(loop_start) ; i<loop_sz_num ; i++ ) {
        string unrolled_body = std::regex_replace(loop_body, index_rx, "[" + to_string(i) + "]") ;
        unrolled_body = std::regex_replace(unrolled_body, var_rx, to_string(i)) ;
        code_ += "\n{" ;
        code_ += unrolled_body ;
        code_ += "\n}\n" ;
    }

}

void OpenGLShader::setSourceFile(const std::string &file_name, const OpenGLShaderPreproc &defines) {
    resource_name_ = file_name ;

    preproc(file_name, defines, false) ;

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

        throw_error(code_, "Error compilining shader: ", info_log) ;
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
        throw_error({}, "Error linking shader program", error_log) ;
    }

    if ( validate ) {
        glValidateProgram(handle_);
        glGetProgramiv(handle_, GL_VALIDATE_STATUS, &success);

        if ( !success ) {
            glGetProgramInfoLog(handle_, sizeof(error_log), NULL, error_log);
            throw_error({}, "Invalid shader program", error_log);
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
} // xviz
