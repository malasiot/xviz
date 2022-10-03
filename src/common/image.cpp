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
    virtual ~ImageData() = default ;
    ImageType type_ ;
};

class RawImageData: public ImageData {
public:

    RawImageData(unsigned char *bytes, ImageFormat fmt, uint32_t w, uint32_t h):
        ImageData(ImageType::Raw), width_(w), height_(h), format_(fmt) {
        uint32_t data_sz = sz()  ;
        bytes_.reset(bytes) ;
      //  bytes_.reset(new unsigned char [data_sz]) ;
      //  std::memcpy(bytes_.get(), bytes, data_sz) ;
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
        case ImageFormat::gray16:
            return width_ * height_ * 2 ;
        case ImageFormat::encoded:
            return width_ ;
        }
    }

    uint32_t width_, height_ ;
    ImageFormat format_ ;
    std::unique_ptr<unsigned char> bytes_ ;
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

Image::Image(unsigned char *bytes, ImageFormat fmt, uint32_t w, uint32_t h) {
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

#ifdef HAS_LIBPNG

#include <png.h>

template<typename W>
bool png_write(const Image &im, W &writer) ;

class PNGStringWriter {

public:

    PNGStringWriter(string &str): data_(str) {}

    static void callback(png_structp  png_ptr, png_bytep data, png_size_t length) {
        PNGStringWriter *p = (PNGStringWriter *)png_get_io_ptr(png_ptr);
        p->data_.append((char *)data, length) ;
    }

    static void flush(png_structp png_ptr) {

    }

    string &data_ ;
};


class PNGFileWriter {

public:

    PNGFileWriter(FILE *file): file_(file) {}

    static void callback(png_structp  png_ptr, png_bytep data, png_size_t length) {
        PNGFileWriter *p = (PNGFileWriter *)png_get_io_ptr(png_ptr);
        fwrite((char *)data, length, 1, p->file_) ;
    }

    static void flush(png_structp png_ptr) {
        PNGFileWriter *p = (PNGFileWriter *)png_get_io_ptr(png_ptr);
        fflush(p->file_) ;
    }

    FILE *file_ ;
};


template <typename W>
bool png_write(const Image &im, W &writer) {
    const unsigned char *pixels = im.data() ;
    assert(pixels) ;
    png_uint_32 width = im.width(), height = im.height(), stride ;
    int bit_depth = 8, color_type ;

    if ( im.format() == ImageFormat::rgb24 ) {
        color_type = PNG_COLOR_TYPE_RGB ;
        stride = width * 3 ;
    } else if ( im.format() == ImageFormat::rgba32 ) {
        color_type = PNG_COLOR_TYPE_RGBA ;
        stride = width * 4 ;
    } else if ( im.format() == ImageFormat::gray16 ) {
        color_type = PNG_COLOR_TYPE_GRAY ;
        stride = width * 2 ;
        bit_depth = 16 ;
    } else if ( im.format() == ImageFormat::gray8 ) {
        color_type = PNG_COLOR_TYPE_GRAY ;
        stride = width ;
    }


    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if ( !png_ptr ) return false ;

    /* Allocate/initialize the image information data.  REQUIRED */
    png_infop info_ptr = png_create_info_struct((png_structp)png_ptr);

    if ( !info_ptr ) {
        //  png_destroy_write_struct((png_structpp)&png_ptr,  png_infopp_NULL);
        png_destroy_write_struct((png_structpp)&png_ptr,  NULL);
        return false ;
    }

    /* Set error handling.  REQUIRED if you aren't supplying your own
      * error handling functions in the png_create_write_struct() call.
      */
    if (setjmp(png_jmpbuf((png_structp)png_ptr))) {
        png_destroy_write_struct((png_structpp)&png_ptr, (png_infopp)&info_ptr);
        return false;
    }

    /* set up the output control if you are using standard C streams */
    //   png_init_io((png_structp)png_ptr, fp);
    png_set_write_fn(png_ptr, &writer, W::callback, W::flush);

    png_set_IHDR((png_structp)png_ptr, (png_infop)info_ptr, width, height, bit_depth, color_type,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info((png_structp)png_ptr, (png_infop)info_ptr);

    png_set_swap(png_ptr);

    png_bytep *row_pointers = new png_bytep [height];

    char *p = (char *)pixels ;

    for (int row = 0; row < height; row++, p += stride ) {
        if ( im.format() == ImageFormat::rgba32 ) {
            png_bytep data = new png_byte [width * 4], dst = data ;
            unsigned char *src = (unsigned char *)p ;
            for( int col = 0 ; col < width ; col++, src += 4 ) {
                *dst++ = src[0] ;
                *dst++ = src[1] ;
                *dst++ = src[2] ;
                *dst++ = src[3] ;
            }

            row_pointers[row] = (png_bytep)data ;
        } else if ( im.format() == ImageFormat::rgb24){
            png_bytep data = new png_byte [width * 3], dst = data ;
            unsigned char *src = (unsigned char *)p ;
            for( int col = 0 ; col < width ; col++, src += 3 ) {
                *dst++ = src[0] ;
                *dst++ = src[1] ;
                *dst++ = src[2] ;
            }

             row_pointers[row] = (png_bytep)data ;

        } else if ( im.format() == ImageFormat::gray16) {
            png_bytep data = new png_byte [width * 2], dst = data ;
            unsigned char *src = (unsigned char *)p ;
            for( int col = 0 ; col < width ; col++, src += 2 ) {
                *dst++ = src[0] ;
                *dst++ = src[1] ;
            }

             row_pointers[row] = (png_bytep)data ;

        }
    }

    png_write_image((png_structp)png_ptr, row_pointers);
    /* It is REQUIRED to call this to finish writing the rest of the file */
    png_write_end((png_structp)png_ptr, (png_infop)info_ptr);

    for (int row = 0; row < height; row++)
        delete [] row_pointers[row] ;

    delete [] row_pointers ;

    /* clean up after the write, and free any memory allocated */
    png_destroy_write_struct((png_structpp)&png_ptr, (png_infopp)&info_ptr);

    return true ;
}

bool Image::saveToPNGBuffer(string &buffer) {
    PNGStringWriter w(buffer) ;
    return png_write(*this, w) ;
}

bool Image::saveToPNG(const string &fpath) {
    FILE *fp = fopen(fpath.c_str(), "wb") ;
    if ( !fp ) return false ;
    PNGFileWriter w(fp) ;
    bool res = png_write(*this, w) ;
    fclose(fp) ;
    return res ;
}
#endif

}
