#include "texture_data.hpp"
#include <xviz/common/image.hpp>
#include <xviz/scene/material.hpp>

#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace xviz { namespace impl {

TextureData::TextureData() {
}

TextureData::~TextureData() {
    if ( id_ )
        glDeleteTextures(1, &id_) ;
}

void tga_write(const char *filename, uint32_t width, uint32_t height, uint8_t *dataBGRA, uint8_t dataChannels=4, uint8_t fileChannels=3)
{
    std::ofstream fp(filename, std::ios::binary) ;

    // You can find details about TGA headers here: http://www.paulbourke.net/dataformats/tga/
    uint8_t header[18] = { 0,0,2,0,0,0,0,0,0,0,0,0, (uint8_t)(width%256), (uint8_t)(width/256), (uint8_t)(height%256), (uint8_t)(height/256), (uint8_t)(fileChannels*8), 0x20 };
    fp.write((char *)&header, 18);

    for (uint32_t i = 0; i < width*height; i++)
    {
        for (uint32_t b = 0; b < fileChannels; b++)
        {
            fp.put(dataBGRA[(i*dataChannels) + (b%dataChannels)]);
        }
    }
}

void TextureData::setupTextureParams(const Sampler2D &sampler) {
    switch ( sampler.wrapModeU() ) {
    case Sampler2D::WRAP_CLAMP:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        break;
    case Sampler2D::WRAP_MIRROR_REPEAT:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        break;
    default:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        break;
    }

    switch ( sampler.wrapModeV() ) {
    case Sampler2D::WRAP_CLAMP:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        break;
    case Sampler2D::WRAP_MIRROR_REPEAT:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        break;
    default:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;
    }

    switch ( sampler.magnification() ) {
    case Sampler2D::MAG_NEAREST:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    default:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    }

    switch ( sampler.minification() ) {
    case Sampler2D::MIN_NEAREST:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;
    case Sampler2D::MIN_LINEAR:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    case Sampler2D::MIN_NEAREST_MIPMAP_NEAREST:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        break;
    case Sampler2D::MIN_NEAREST_MIPMAP_LINEAR:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        break;
    case Sampler2D::MIN_LINEAR_MIPMAP_NEAREST:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        break;
    default:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;
    }
}

bool TextureData::create(Image *image, const Sampler2D &sampler)
{
    stbi_set_flip_vertically_on_load(true);

    if ( image->type() == ImageType::Uri ) {
        std::string path = image->uri() ;

        int width, height, channels;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if ( !data ) return false ;

        glGenTextures(1, &id_);
        glBindTexture(GL_TEXTURE_2D, id_) ;

        if ( channels == 3 )
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else if ( channels == 4 )
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        setupTextureParams(sampler) ;

        stbi_image_free(data);

        loaded_ = true ;

    } else if ( image->type() == ImageType::Raw ) {
        if ( image->format() == ImageFormat::encoded ) {

            int width, height, channels;
            unsigned char *data = stbi_load_from_memory(image->data(), image->width(), &width, &height, &channels, 0);

            if ( !data ) return false ;

            glGenTextures(1, &id_);
            glBindTexture(GL_TEXTURE_2D, id_) ;

            if ( channels == 3 )
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            else if ( channels == 4 )
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            glGenerateMipmap(GL_TEXTURE_2D);

            setupTextureParams(sampler) ;

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

TextureData *TextureCache::fetch(Image *im, const Sampler2D &sampler) {
    auto it = data_.find(im) ;
    if ( it != data_.end() )
        return it->second.get() ;
    else {
        std::unique_ptr<TextureData> data(new TextureData) ;

        data->cache_ = this ;
        data->image_ = im ;

        if ( data->create(im, sampler) ) {
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
