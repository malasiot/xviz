#include <xviz/font.hpp>

#include "session.pb.h"

namespace xviz {

msg::Font *Font::write(const FontHandle &f) {
    msg::Font *msg = new msg::Font ;
    for ( const auto &fam: f->familyNames() ) {
        msg->add_family(fam) ;
    }
    switch ( f->style() ) {
    case FontStyle::Normal:
        msg->set_style(msg::Font::FONT_STYLE_NORMAL) ;
        break ;
    case FontStyle::Oblique:
        msg->set_style(msg::Font::FONT_STYLE_OBLIQUE) ;
        break ;
    case FontStyle::Italic:
        msg->set_style(msg::Font::FONT_STYLE_ITALIC) ;
        break ;
    }

    msg->set_sz(f->size()) ;

    return msg ;
}

FontHandle Font::read(const msg::Font &msg) {
    FontHandle f(new Font()) ;

    std::string families ;
    for( const std::string &fam: msg.family()) {
        if ( !families.empty() ) families.push_back(';') ;
        families.append(fam) ;
    }

    f->setFamily(families) ;

    f->setSize(msg.sz()) ;

    return f ;
}




}
