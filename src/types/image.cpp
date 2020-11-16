#include <xviz/image.hpp>

#include "session.pb.h"

#include <cstring>

using namespace std ;

namespace xviz {

class ImageData {

};

class RawImageData: public ImageData {
public:

    RawImageData(const unsigned char *bytes, ImageFormat fmt, uint32_t w, uint32_t h): width_(w), height_(h), format_(fmt) {
        uint32_t data_sz = sz()  ;
        bytes_.reset(new unsigned char [data_sz]) ;
        std::memcpy(bytes_.get(), bytes, data_sz) ;
    }

    uint32_t sz() const {
        switch ( format_ ) {
        case ImageFormat::rgba32:
            return width_ * height_ * 4 ;
        case ImageFormat::rgb24:
            return width_ * height_ * 3 ;
        case ImageFormat::gray8:
            return width_ * height_ * 1 ;
        }
    }

    uint32_t width_, height_ ;
    ImageFormat format_ ;
    std::unique_ptr<unsigned char []> bytes_ ;

    ~RawImageData() {}
};

class UriImageData: public ImageData {
public:
    UriImageData(const std::string &str): uri_(str) {}
    std::string uri_ ;
};

Image::Image(): type_(ImageType::NoImage) {}

Image::Image(const string &uri): type_(ImageType::Uri) {
    data_.reset(new UriImageData(uri)) ;
}

Image::Image(const unsigned char *bytes, ImageFormat fmt, uint32_t w, uint32_t h): type_(ImageType::Raw) {
    data_.reset(new RawImageData(bytes, fmt, w, h)) ;
}

string Image::uri() const {
    assert( type_ == ImageType::Uri && data_ ) ;
    return static_cast<UriImageData *>(data_.get())->uri_ ;
}

uint32_t Image::width() const {
    assert( type_ == ImageType::Raw && data_ ) ;
    return static_cast<RawImageData *>(data_.get())->width_ ;
}

uint32_t Image::height() const {
    assert( type_ == ImageType::Raw && data_ ) ;
    return static_cast<RawImageData *>(data_.get())->height_ ;
}

ImageFormat Image::format() const {
    assert( type_ == ImageType::Raw && data_ ) ;
    return static_cast<RawImageData *>(data_.get())->format_ ;
}

const unsigned char *Image::data() const {
    assert( type_ == ImageType::Raw && data_ ) ;
    return static_cast<RawImageData *>(data_.get())->bytes_.get() ;
}


uint32_t Image::dataSize() const {
    assert( type_ == ImageType::Raw && data_ ) ;
    return static_cast<RawImageData *>(data_.get())->sz() ;
}


Image Image::read(const msg::Image &data) {
    if ( data.has_image_uri() ) {
        return Image(data.image_uri()) ;
    } else if ( data.has_raw_image() ) {
        const msg::ImageRaw &ri = data.raw_image() ;
        ImageFormat fmt ;
        switch ( ri.pixel_type() ) {
        case msg::ImageRaw_PixelType_RGBA32:
            fmt = ImageFormat::rgba32 ;
            break ;
        case msg::ImageRaw_PixelType_RGB24:
            fmt = ImageFormat::rgb24 ;
            break ;
        case msg::ImageRaw_PixelType_GRAY8:
            fmt = ImageFormat::gray8 ;
            break ;
        }

        return Image(reinterpret_cast<const unsigned char *>(ri.pixels().data()), fmt, ri.width(), ri.height()) ;
    } else { return Image() ; }
}

msg::Image * Image::write(const Image &c) {
    msg::Image *msg_im = new msg::Image() ;
    if ( c.type() == ImageType::Raw ) {
        msg::ImageRaw *msg_raw = new msg::ImageRaw ;
        switch ( c.format() ) {
        case ImageFormat::rgba32:
            msg_raw->set_pixel_type(msg::ImageRaw_PixelType_RGBA32);
            break ;
        case ImageFormat::rgb24:
            msg_raw->set_pixel_type(msg::ImageRaw_PixelType_RGB24);
            break ;
        case ImageFormat::gray8:
            msg_raw->set_pixel_type(msg::ImageRaw_PixelType_GRAY8);
            break ;
        }
        msg_raw->set_width(c.width()) ;
        msg_raw->set_height(c.height()) ;
        msg_raw->set_pixels(c.data(), c.dataSize()) ;
        msg_im->set_allocated_raw_image(msg_raw) ;
    } else if ( c.type() == ImageType::Uri ) {
        msg_im->set_image_uri(c.uri()) ;
    }
    return msg_im;
}

}
