#include <xviz/image.hpp>

#include "session.pb.h"

#include <cstring>
#include <iostream>

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

    ~RawImageData() {}
};

class UriImageData: public ImageData {
public:
    UriImageData(const std::string &str): ImageData(ImageType::Uri), uri_(str) {}
    std::string uri_ ;

    ~UriImageData() {
        cout << "ok" << endl ;
    }
};

Image::Image() {}

Image::Image(const string &uri) {
    data_.reset(new UriImageData(uri)) ;
}

Image::Image(const unsigned char *bytes, ImageFormat fmt, uint32_t w, uint32_t h) {
    data_.reset(new RawImageData(bytes, fmt, w, h)) ;
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

string Image::encode() const {
    std::unique_ptr<msg::Image> im_msg(Image::write(*this)) ;
    return im_msg->SerializeAsString();
}

Image *Image::decode(const string &payload) {
    msg::Image im_msg ;
    if ( !im_msg.ParseFromString(payload) ) return nullptr ;
    return Image::read(im_msg) ;
}

Image *Image::read(const msg::Image &data) {
    if ( data.has_image_uri() ) {
        return new Image(data.image_uri()) ;
    } else if ( data.has_raw_image() ) {
        const msg::ImageRaw &ri = data.raw_image() ;
        ImageFormat fmt ;
        switch ( ri.format() ) {
        case msg::ImageRaw_Format_RGBA32:
            fmt = ImageFormat::rgba32 ;
            break ;
        case msg::ImageRaw_Format_RGB24:
            fmt = ImageFormat::rgb24 ;
            break ;
        case msg::ImageRaw_Format_GRAY8:
            fmt = ImageFormat::gray8 ;
            break ;
        case msg::ImageRaw_Format_ENCODED:
            fmt = ImageFormat::encoded ;
            break ;
        }

        return new Image(reinterpret_cast<const unsigned char *>(ri.pixels().data()), fmt, ri.width(), ri.height()) ;
    } else { return new Image() ; }
}

msg::Image * Image::write(const Image &c) {
    msg::Image *msg_im = new msg::Image() ;
    if ( c.type() == ImageType::Raw ) {
        msg::ImageRaw *msg_raw = new msg::ImageRaw ;

        switch ( c.format() ) {
        case ImageFormat::rgba32:
            msg_raw->set_format(msg::ImageRaw_Format_RGBA32);
            break ;
        case ImageFormat::rgb24:
            msg_raw->set_format(msg::ImageRaw_Format_RGB24);
            break ;
        case ImageFormat::gray8:
            msg_raw->set_format(msg::ImageRaw_Format_GRAY8);
            break ;
        case ImageFormat::encoded:
            msg_raw->set_format(msg::ImageRaw_Format_ENCODED);
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
