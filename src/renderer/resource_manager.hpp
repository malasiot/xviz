#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <map>

namespace clsim { namespace impl {

class OpenGLShader ;

class OpenGLShaderResourceManager {
public:

    static void setShaderResourceFolder(const std::string &folder) ;

private:
    friend class OpenGLShader ;

    // load resource (internal or file)
    static std::string fetch(const std::string &name) ;

    OpenGLShaderResourceManager() ;

    void addSource(const char *name, const char *src) ;

    static OpenGLShaderResourceManager instance_ ;

    std::map<std::string, std::string> sources_ ;
    std::string folder_ ;
};

}}

#endif
