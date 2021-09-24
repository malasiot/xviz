#ifndef XVIZ_TEXTURE_DATA_HPP
#define XVIZ_TEXTURE_DATA_HPP

#include <string>
#include <memory>

#include "gl/gl3w.h"

namespace xviz { namespace impl {

class TextureData {
public:
    TextureData() ;
    ~TextureData() ;

    bool create(const std::string &path) ;

    GLuint id() const { return id_ ; }

private:
    GLuint id_ ;
};

}}

#endif
