#ifndef XVIZ_TEXTURE_DATA_HPP
#define XVIZ_TEXTURE_DATA_HPP

#include <string>
#include <memory>

#include "gl/gl3w.h"
#include <map>

#include <xviz/common/image.hpp>
#include <xviz/scene/material.hpp>

namespace xviz { namespace impl {

class TextureData {
public:
    TextureData() ;
    ~TextureData() ;

    bool create(Image *image, const Sampler2D &sampler) ;

    void setupTextureParams(const Sampler2D &sampler) ;

    GLuint id() const { return id_ ; }
    bool loaded() const { return loaded_ ; }

    void release() ;

private:
    friend class TextureCache ;

    bool loaded_ = false ;
    GLuint id_ ;
    TextureCache *cache_ = nullptr ;
    Image *image_ = nullptr ;
};

class TextureCache {
public:
    TextureData *fetch(Image *im, const Sampler2D &sampler) ;
    void release(Image *im) ;

    ~TextureCache() ;

private:
    std::map<Image *, std::unique_ptr<TextureData>> data_ ;
};

}}

#endif
