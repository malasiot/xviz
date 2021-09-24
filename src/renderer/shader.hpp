#ifndef XVIZ_GL_SHADER_HPP
#define XVIZ_GL_SHADER_HPP

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <map>

#include <Eigen/Core>
#include <memory>

namespace xviz { namespace impl {


enum OpenGLShaderType { VERTEX_SHADER,  FRAGMENT_SHADER, GEOMETRY_SHADER, COMPUTE_SHADER, TESS_CONTROL_SHADER, TESS_EVALUATION_SHADER  } ;

struct OpenGLShaderPreproc {

    void appendDefinition(const std::string &def) {
        defines_.push_back(def) ;
    }

    void appendConstant(const std::string &name, const std::string &val) {
        constants_.emplace(name, val) ;
    }

private:

    friend class OpenGLShader ;

    std::map<std::string, std::string> constants_ ;
    std::vector<std::string> defines_ ;
};

class OpenGLShader {
public:

    OpenGLShader(OpenGLShaderType t) { create(t) ; }

    void setHeader(const std::string &header) ;
    void addPreProcDefinition(const std::string &key, const std::string &val = std::string()) ;

    // Compile shader from source code string.
    void setSourceCode(const std::string &code) ;
    // Loads the code from the designated file or internal resource string and calls compile.
    // Internal resources are designated by prepending the filename with '@'.
    // A preprocessor is run on the loaded resource. This will resolve any #include directives,
    // prepend any defines, or replace any defined strings with their value
    void setSourceFile(const std::string &fileName, const OpenGLShaderPreproc &defines = {}) ;


    ~OpenGLShader() ;

    void compile() ;

    bool isCompiled() const { return compiled_ ; }

    unsigned int handle() const { return handle_; }

private:

    void create(OpenGLShaderType t) ;
    void preproc(const std::string &filename, const OpenGLShaderPreproc &defines, bool version_parsed) ;

    std::string header_ = "#version 330\n" ;
    std::string preproc_ ;

    unsigned int handle_ ;
    bool compiled_ = false ;

    std::string code_ ;
    std::string resource_name_ ;
    OpenGLShaderType type_ ;
};

using OpenGLShaderPtr = std::shared_ptr<OpenGLShader> ;

class OpenGLShaderProgram {
public:

    typedef std::shared_ptr<OpenGLShaderProgram> Ptr ;

    OpenGLShaderProgram() ;

    void addShader(const OpenGLShaderPtr &shader) ;
    void addShaderFromCode(OpenGLShaderType t, const std::string &code) ;
    void addShaderFromFile(OpenGLShaderType t, const std::string &fname, const OpenGLShaderPreproc &preproc = {}) ;

    void link(bool validate = true) ;
    void use() ;

    int attributeLocation(const std::string &attr_name) ;
    void bindAttributeLocation(const std::string &attr_name, int loc) ;

    int uniformLocation(const std::string &uni_name) ;

    OpenGLShaderProgram(const char *vshader, const char *fshader) ;

    void setUniform(const std::string &name, float v) ;
    void setUniform(const std::string &name, int v) ;
    void setUniform(const std::string &name, uint v) ;
    void setUniform(const std::string &name, const Eigen::Vector2f &v);
    void setUniform(const std::string &name, const Eigen::Vector3f &v) ;
    void setUniform(const std::string &name, const Eigen::Vector4f &v) ;
    void setUniform(const std::string &name, const Eigen::Matrix3f &v) ;
    void setUniform(const std::string &name, const Eigen::Matrix4f &v) ;

    ~OpenGLShaderProgram() ;

    unsigned int handle() const { return handle_ ; }


private:

    void throwError(const char *error_str, const char *error_desc) ;

    unsigned int handle_ ;
    std::vector<OpenGLShaderPtr> shaders_ ;
};


class OpenGLShaderError: public std::runtime_error {
public:
    OpenGLShaderError(const std::string &msg);
};


} // internal
} // clsim

#endif
