#ifndef XVIZ_RESOURCE_LOADER_HPP
#define XVIZ_RESOURCE_LOADER_HPP

#include <string>
#include <functional>

#include <xviz/common/image.hpp>

namespace xviz {


class ResourceLoader {
public:
    ResourceLoader() ;

    using load_cb_t = std::function<void(const Image &im)> ;

    virtual void loadTexture(const std::string &path, load_cb_t cb) ;
};

}





#endif
