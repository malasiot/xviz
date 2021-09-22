#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <map>

namespace clsim { namespace impl {

class ShaderResourceManager {
public:

    static const char *fetch(const std::string &name) ;

private:

    ShaderResourceManager() ;

    void addSource(const char *name, const char *src) ;

    static ShaderResourceManager instance_ ;

    std::map<std::string, std::string> sources_ ;
};

}}

#endif
