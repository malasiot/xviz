#include <xviz/scene/material.hpp>

#include "scene.pb.h"

namespace xviz {

msg::Sampler2D *Sampler2D::write(const Sampler2D &s) {
   msg::Sampler2D *msg = new msg::Sampler2D ;

   switch ( s.mapModeU() ) {
   case Sampler2D::CLAMP:
       msg->set_wrap_u(msg::Sampler2D_TextureMapMode_CLAMP);
       break ;
   case Sampler2D::DECAL:
       msg->set_wrap_u(msg::Sampler2D_TextureMapMode_DECAL);
       break ;
   case Sampler2D::WRAP:
       msg->set_wrap_u(msg::Sampler2D_TextureMapMode_WRAP);
       break ;
   }

   switch ( s.mapModeV() ) {
   case Sampler2D::CLAMP:
       msg->set_wrap_v(msg::Sampler2D_TextureMapMode_CLAMP);
       break ;
   case Sampler2D::DECAL:
       msg->set_wrap_v(msg::Sampler2D_TextureMapMode_DECAL);
       break ;
   case Sampler2D::WRAP:
       msg->set_wrap_v(msg::Sampler2D_TextureMapMode_WRAP);
       break ;
   }

   return msg ;

}

msg::Texture2D *Texture2D::write(const Texture2D &t)  {
    msg::Texture2D *msg = new msg::Texture2D ;

    msg->set_allocated_image(Image::write(t.image())) ;
    msg->set_allocated_sampler(Sampler2D::write(t.sampler()));

    return msg ;

}
Texture2D *Texture2D::read(const msg::Texture2D &msg) {

}
}
