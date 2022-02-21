#include "texture_data.hpp"
#include <xviz/common/image.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace xviz { namespace impl {

TextureData::TextureData() {
}

TextureData::~TextureData() {
    if ( id_ )
        glDeleteTextures(1, &id_) ;
}

bool TextureData::create(Image *image)
{
    stbi_set_flip_vertically_on_load(true);

    if ( image->type() == ImageType::Uri ) {
        std::string path = image->uri() ;

        int width, height, channels;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if ( !data ) return false ;

        glGenTextures(1, &id_);
        glBindTexture(GL_TEXTURE_2D, id_) ;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        loaded_ = true ;

    } else if ( image->type() == ImageType::Raw ) {
        if ( image->format() == ImageFormat::encoded ) {

            int width, height, channels;
            unsigned char *data = stbi_load_from_memory(image->data(), image->width(), &width, &height, &channels, 0);

            if ( !data ) return false ;

            glGenTextures(1, &id_);
            glBindTexture(GL_TEXTURE_2D, id_) ;

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);

            loaded_ = true ;
        }
    }

    if ( loaded_ ) image_ = image ;

    return loaded_ ;
}

void TextureData::release() {
    cache_->release(image_) ;
}

TextureData *TextureCache::fetch(Image *im) {
    auto it = data_.find(im) ;
    if ( it != data_.end() )
        return it->second.get() ;
    else {
        std::unique_ptr<TextureData> data(new TextureData) ;

        data->cache_ = this ;
        data->image_ = im ;

        if ( data->create(im) ) {
            auto res = data_.emplace(im, std::move(data)) ;
            im->texture_ = res.first->second.get() ;

            return im->texture_ ;
        }
        else {
            return nullptr ;
        }
    }
}

void TextureCache::release(Image *im) {
    data_.erase(im) ;
    im->texture_ = nullptr ;
}

TextureCache::~TextureCache() {
    for( auto &p: data_ ) {
        Image *im = p.first ;
        im->texture_ = nullptr ;
    }
}

}}
