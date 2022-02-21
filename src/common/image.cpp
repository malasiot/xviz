#include <xviz/common/image.hpp>

#include <cstring>
#include <iostream>
#include <cassert>

#include "../renderer/texture_data.hpp"

using namespace std ;

namespace xviz {

class ImageData {
public:
    ImageData(ImageType t): type_(t) {}
    ImageType type_ ;
};

class RawImageData: public ImageData {
public:

    RawImageData(const unsigned char *bytes, ImageFormat fmt, uint32_t w, uint32_t h):
        ImageData(ImageType::Raw), width_(w), height_(h), format_(fmt) {
        uint32_t data_sz = sz()  ;
        bytes_.reset(new unsigned char [data_sz]) ;
        std::memcpy(bytes_.get(), bytes, data_sz) ;
      //  uuid_ = sole::uuid0().str() ;
    }

    uint32_t sz() const {
        switch ( format_ ) {
        case ImageFormat::rgba32:
            return width_ * height_ * 4 ;
        case ImageFormat::rgb24:
            return width_ * height_ * 3 ;
        case ImageFormat::gray8:
            return width_ * height_ * 1 ;
        case ImageFormat::encoded:
            return width_ ;
        }
    }

    uint32_t width_, height_ ;
    ImageFormat format_ ;
    std::unique_ptr<unsigned char []> bytes_ ;
    std::string uuid_ ;

    ~RawImageData() {}
};

class UriImageData: public ImageData {
public:
    UriImageData(const std::string &str): ImageData(ImageType::Uri), uri_(str) {}
    std::string uri_ ;

    ~UriImageData() {

    }
};

Image::Image() {}

Image::Image(const string &uri) {
    data_.reset(new UriImageData(uri)) ;
}

Image::Image(const unsigned char *bytes, ImageFormat fmt, uint32_t w, uint32_t h) {
    data_.reset(new RawImageData(bytes, fmt, w, h)) ;
}

Image::~Image() {
    if ( texture_ ) texture_->release() ;
}

ImageType Image::type() const {
    if ( !data_ ) return ImageType::NoImage ;
    else return data_->type_ ;
}

string Image::uri() const {
    assert( type() == ImageType::Uri && data_ ) ;
    return static_cast<UriImageData *>(data_.get())->uri_ ;
}

uint32_t Image::width() const {
    assert( type() == ImageType::Raw && data_ ) ;
    return static_cast<RawImageData *>(data_.get())->width_ ;
}

uint32_t Image::height() const {
    assert( type() == ImageType::Raw && data_ ) ;
    return static_cast<RawImageData *>(data_.get())->height_ ;
}

ImageFormat Image::format() const {
    assert( type() == ImageType::Raw && data_ ) ;
    return static_cast<RawImageData *>(data_.get())->format_ ;
}

const unsigned char *Image::data() const {
    assert( type() == ImageType::Raw && data_ ) ;
    return static_cast<RawImageData *>(data_.get())->bytes_.get() ;
}

uint32_t Image::dataSize() const {
    assert( type() == ImageType::Raw && data_ ) ;
    if ( format() == ImageFormat::encoded )
        return width();
    else
        return static_cast<RawImageData *>(data_.get())->sz() ;
}

std::string Image::id() const {
    if ( type() == ImageType::Raw ) return static_cast<RawImageData *>(data_.get())->uuid_ ;
    else if ( type() == ImageType::Uri ) return static_cast<UriImageData *>(data_.get())->uri_ ;
    else return {};

}


}
