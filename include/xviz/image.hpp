#ifndef XVIZ_IMAGE_HPP
#define XVIZ_IMAGE_HPP

#include <cinttypes>
#include <string>
#include <memory>

#include <xviz/message.hpp>

namespace xviz {

namespace msg {
    class Image ;
}

enum class ImageType { Raw, Uri, NoImage } ;

enum class ImageFormat { rgb24, rgba32, gray8, encoded };
/* encoded means an encoded image payload  e.g. PNG.
 * In this case width is the data size and height is 0 */

class ImageData ;

class Image: public Message {
public:

    // Null image
    Image() ;

    // Image from uri
    Image(const std::string &uri) ;

    // Image from raw bytes
    Image(const unsigned char *bytes, ImageFormat fmt, uint32_t w, uint32_t h) ;

    ImageType type() const ;

    // get image location if it is of type Uri
    std::string uri() const ;

    // getters for raw images
    uint32_t width() const ;
    uint32_t height() const ;
    ImageFormat format() const ;
    const unsigned char *data() const ;
    uint32_t dataSize() const ;

    std::string encode() const override ;
    static Image *decode(const std::string &payload) ;

    // i/o
    static Image *read(const msg::Image &) ;
    static msg::Image *write(const Image &) ;


private:

    std::shared_ptr<ImageData> data_ ;
};


}

#endif
