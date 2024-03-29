#ifndef XVIZ_IMAGE_HPP
#define XVIZ_IMAGE_HPP

#include <cinttypes>
#include <string>
#include <memory>

namespace xviz {

enum class ImageType { Raw, Uri, NoImage } ;

enum class ImageFormat { rgb24, rgba32, gray8, gray16, float32, encoded };
/* encoded means an encoded image payload  e.g. PNG.
 * In this case width is the data size and height is 0 */

class ImageData ;

namespace impl {
class TextureData ;
class TextureCache ;
}

class Image {
public:

    // Null image
    Image() ;

    // Image from uri
    Image(const std::string &uri) ;

    // Image from raw bytes
    Image(unsigned char *bytes, ImageFormat fmt, uint32_t w, uint32_t h) ;

    ~Image() ;

    ImageType type() const ;

    // get image location if it is of type Uri
    std::string uri() const ;

    // getters for raw images
    uint32_t width() const ;
    uint32_t height() const ;
    ImageFormat format() const ;
    const unsigned char *data() const ;
    uint32_t dataSize() const ;

    // unique id used for resource management
    std::string id() const ;

#ifdef HAS_LIBPNG
    bool saveToPNG(const std::string &fileName) ;
    bool saveToPNGBuffer(std::string &data);
#endif

private:

    friend class impl::TextureCache ;

    std::shared_ptr<ImageData> data_ ;
    impl::TextureData *texture_ = nullptr ;
};

using ImagePtr = std::shared_ptr<Image> ;

}

#endif
