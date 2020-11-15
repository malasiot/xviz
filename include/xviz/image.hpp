#ifndef XVIZ_IMAGE_HPP
#define XVIZ_IMAGE_HPP

#include <cinttypes>
#include <string>
#include <memory>

#include <xviz/serialize.hpp>

namespace xviz {

namespace msg {
    class Image ;
}


class Image {
public:
    virtual ~Image() = default ;

    static Image *read(const msg::Image &) ;
    static msg::Image *write(const Image *) ;
};

class RawImage: public Image {
public:
    enum PixelType {
        RGB, RGBA
    };

    PixelType type_ ;
    uint32_t width_, height_, stride_ ;
    std::unique_ptr<char []> bytes_ ;
};

class ImageUri: public Image {
public:
    ImageUri(const std::string &uri): uri_(uri) {}

    std::string uri_ ;
};


}

#endif
