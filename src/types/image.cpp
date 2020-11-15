#include <xviz/image.hpp>

#include "session.pb.h"
using namespace std ;

namespace xviz {

Image *Image::read(const msg::Image &data) {
    if ( data.has_image_uri() ) {
        return new ImageUri(data.image_uri()) ;
    }
}

msg::Image * Image::write(const Image *c) {
    msg::Image *msg_im = new msg::Image() ;
    if ( dynamic_cast<const RawImage *>(c) ) {
        return nullptr ;
    } else if ( const ImageUri *im = dynamic_cast<const ImageUri *>(c) ) {
        msg_im->set_image_uri(im->uri_) ;
    }
    return msg_im;
}

}
