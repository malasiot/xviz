#include "texture_data.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace clsim { namespace impl {

TextureData::TextureData() {
}

TextureData::~TextureData() {
    if ( id_ )
        glDeleteTextures(1, &id_) ;
}

bool TextureData::create(const std::string &path)
{
    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if ( !data ) return false ;

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_) ;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return true ;
}

}}
