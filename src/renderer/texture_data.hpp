#ifndef XVIZ_TEXTURE_DATA_HPP
#define XVIZ_TEXTURE_DATA_HPP

#include <string>
#include <memory>

#include "gl/gl3w.h"

#include <xviz/common/image.hpp>


namespace xviz { namespace impl {

class TextureData {
public:
    TextureData() ;
    ~TextureData() ;

    bool create(const Image &image) ;

    GLuint id() const { return id_ ; }
    bool loaded() const { return loaded_ ; }

private:
    bool loaded_ = false ;
    GLuint id_ ;
};

}}

#endif
