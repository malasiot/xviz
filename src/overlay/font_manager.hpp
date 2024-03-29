#ifndef XVIZ_FONT_MANAGER_HPP
#define XVIZ_FONT_MANAGER_HPP

#include <xviz/common/font.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <mutex>
#include <map>

namespace xviz { namespace impl {

class FontManager {
public:

    FT_Face find(const std::string &key);

    void save(const std::string &key, FT_Face face);

    static FontManager &instance() {
        static FontManager s_instance ;
        return s_instance ;
    }

    FT_Face queryFontFace(const Font &font) ;

    // Use FreeType library and FontConfig to query system for desired font
    FT_Face queryFace(const std::string &family_name, FontStyle font_style, FontWeight font_weight) ;

private:

    FontManager() ;
    ~FontManager() ;

    static std::string font_face_key(const std::string &family_name, FontStyle font_style, FontWeight font_weight) ;

    std::map<std::string, FT_Face> cache_ ;
    std::mutex mx_ ;
    FT_Library  library_ ;
} ;

}}


#endif
