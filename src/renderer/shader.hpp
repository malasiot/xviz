#ifndef CLSIM_GL_SHADER_HPP
#define CLSIM_GL_SHADER_HPP

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <map>

#include <Eigen/Core>
#include <memory>

namespace clsim { namespace impl {


enum OpenGLShaderType { VERTEX_SHADER,  FRAGMENT_SHADER, GEOMETRY_SHADER, COMPUTE_SHADER, TESS_CONTROL_SHADER, TESS_EVALUATION_SHADER  } ;

class OpenGLShader {
public:

    OpenGLShader(OpenGLShaderType t) { create(t) ; }

    void setHeader(const std::string &header) ;
    void addPreProcDefinition(const std::string &key, const std::string &val = std::string()) ;

    // Compile shader from source code string. A resource name may be passed to be able to identify the code in error messages
    void addSourceString(const std::string &code, const std::string &resource_name = std::string()) ;
    // Loads the code from the designated file and calls compile string. If no resource name the filename will be used.
    void addSourceFile(const std::string &fileName, const std::string &resource_name = std::string()) ;

    // creates and compiles shader
    OpenGLShader(OpenGLShaderType t, const std::string &code, const std::string &resource_name = std::string()) ;
    ~OpenGLShader() ;

    void compile() ;

    bool isCompiled() const { return compiled_ ; }

    unsigned int handle() const { return handle_; }

private:

    void create(OpenGLShaderType t) ;

    struct Source {
        Source(uint id, std::string name, std::string data): id_(id), name_(name), data_(data) {}
        uint id_ ;
        std::string name_ ;
        std::string data_ ;
    };

    std::string header_ = "#version 330\n" ;
    std::string preproc_ ;

    unsigned int handle_ ;
    bool compiled_ = false ;

    std::vector<Source> sources_ ;
    OpenGLShaderType type_ ;
};

using OpenGLShaderPtr = std::shared_ptr<OpenGLShader> ;

class OpenGLShaderProgram {
public:

    typedef std::shared_ptr<OpenGLShaderProgram> Ptr ;

    OpenGLShaderProgram() ;

    void addShader(const OpenGLShaderPtr &shader) ;
    void addShaderFromString(OpenGLShaderType t, const std::string &code, const std::string &resource_name = std::string()) ;
    void addShaderFromFile(OpenGLShaderType t, const std::string &fname, const std::string &resource_name = std::string()) ;

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
