#ifndef CLSIM_RESOURCE_LOADER_HPP
#define CLSIM_RESOURCE_LOADER_HPP

#include <string>
#include <functional>

#include <clsim/common/image.hpp>

namespace clsim {


class ResourceLoader {
public:
    ResourceLoader() ;

    using load_cb_t = std::function<void(const Image &im)> ;

    virtual void loadTexture(const std::string &path, load_cb_t cb) ;
};

}





#endif
