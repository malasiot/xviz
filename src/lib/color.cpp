#include <xviz/color.hpp>

#include <regex>
#include <cmath>

#include <session.pb.h>

using namespace std ;

namespace xviz {

struct ColorPair {
    const char *name;
    unsigned int r, g, b;
};

class ColorComp {
public:
    ColorComp() {}

    bool operator () ( const ColorPair &c, const std::string &name) const {
        return strcasecmp(c.name, name.c_str() ) < 0;
    }

    bool operator () ( const std::string &name, const ColorPair &c) const {
        return strcasecmp(name.c_str(), c.name ) < 0;
    }
};

bool operator < (const ColorPair &c1, const ColorPair &c2)
{
      return strcasecmp(c1.name, c2.name);
}

static int css_clip_rgb_percent (double in_percent)
{
    /* spec says to clip these values */
    if (in_percent > 100.) return 255;
    else if (in_percent <= 0.) return 0;
    return (int) floor (255. * in_percent / 100. + 0.5);
}

static int css_clip_rgb (int rgb)
{
    /* spec says to clip these values */
    if (rgb > 255) return 255;
    else if (rgb < 0) return 0;
    return rgb;
}

static bool parse_css_color(const string &str, unsigned char &r, unsigned char &g, unsigned char &b) {
    static regex clr_regex_1("#([0-9a-fA-F])([0-9a-fA-F])([0-9a-fA-F])(?:([0-9a-fA-F])([0-9a-fA-F])([0-9a-fA-F]))?", std::regex::icase) ;
    static regex clr_regex_2("rgb\\([\\s]*([0-9]+)[\\s]*,[\\s]*([0-9]+)[\\s]*,[\\s]*([0-9]+)[\\s]*\\)[\\s]*", std::regex::icase) ;
    static regex clr_regex_3("rgb\\([\\s]*([0-9]+)\\%[\\s]*,[\\s]*([0-9]+)\\%[\\s]*,[\\s]*([0-9]+)[\\s]*\\)[\\s]*", std::regex::icase) ;

    smatch what;

    unsigned int val = 0 ;

    if ( regex_match(str, what, clr_regex_1) ) {
        unsigned int hex, i ;

        for( i=1 ; i<what.size() ; i++ )
        {
            string s = what[i] ;

            if ( s.empty() ) break ;

            char c = s.at(0) ;

            if ( c >= '0' && c <= '9' ) hex = c - '0';
            else if ( c >= 'A' && c <= 'F' ) hex = c - 'A' + 10 ;
            else if ( c >= 'a' && c <= 'f' ) hex = c - 'a' + 10 ;

            val = (val << 4) + hex;
        }

        if ( i == 4 ) {
            r = static_cast<uint8_t>(((val & 0xf00) >> 4) | ((val & 0xf00) >> 8)) ;
            g = static_cast<uint8_t>(( val & 0xf0 ) | ( ( val & 0xf0 ) >> 4))  ;
            b = static_cast<uint8_t>((val & 0xf) | ((val & 0xf) << 4)) ;
        } else {
            r = static_cast<uint8_t>((val & 0xff0000) >> 16) ;
            g = static_cast<uint8_t>((val & 0xff00) >> 8)  ;
            b = static_cast<uint8_t>(val & 0xff) ;
        }
        return true ;
    }
    else if ( regex_match(str, what, clr_regex_2) ) {
        r = css_clip_rgb(atoi(what.str(1).c_str())) ;
        g = css_clip_rgb(atoi(what.str(2).c_str())) ;
        b = css_clip_rgb(atoi(what.str(3).c_str())) ;

        return true ;
    }
    else if ( regex_match(str, what, clr_regex_3) )  {
        r = css_clip_rgb_percent(atoi(what.str(1).c_str())) ;
        g = css_clip_rgb_percent(atoi(what.str(2).c_str())) ;
        b = css_clip_rgb_percent(atoi(what.str(3).c_str())) ;

        return true ;
    }
    else { // try named colors

        static const ColorPair color_list[] = {
            {"aliceblue", 240, 248, 255},
            {"antiquewhite", 250, 235, 215},
            {"aqua", 0, 255, 255},
            {"aquamarine", 127, 255, 212},
            {"azure", 240, 255, 255},
            {"beige", 245, 245, 220},
            {"bisque", 255, 228, 196},
            {"black", 0, 0, 0},
            {"blanchedalmond", 255, 235, 205},
            {"blue", 0, 0, 255},
            {"blueviolet", 138, 43, 226},
            {"brown", 165, 42, 42},
            {"burlywood", 222, 184, 135},
            {"cadetblue", 95, 158, 160},
            {"chartreuse", 127, 255, 0},
            {"chocolate", 210, 105, 30},
            {"coral", 255, 127, 80},
            {"cornflowerblue", 100, 149, 237},
            {"cornsilk", 255, 248, 220},
            {"crimson", 220, 20, 60},
            {"cyan", 0, 255, 255},
            {"darkblue", 0, 0, 139},
            {"darkcyan", 0, 139, 139},
            {"darkgoldenrod", 184, 132, 11},
            {"darkgray", 169, 169, 169},
            {"darkgreen", 0, 100, 0},
            {"darkgrey", 169, 169, 169},
            {"darkkhaki", 189, 183, 107},
            {"darkmagenta", 139, 0, 139},
            {"darkolivegreen", 85, 107, 47},
            {"darkorange", 255, 140, 0},
            {"darkorchid", 153, 50, 204},
            {"darkred", 139, 0, 0},
            {"darksalmon", 233, 150, 122},
            {"darkseagreen", 143, 188, 143},
            {"darkslateblue", 72, 61, 139},
            {"darkslategray", 47, 79, 79},
            {"darkslategrey", 47, 79, 79},
            {"darkturquoise", 0, 206, 209},
            {"darkviolet", 148, 0, 211},
            {"deeppink", 255, 20, 147},
            {"deepskyblue", 0, 191, 255},
            {"dimgray", 105, 105, 105},
            {"dimgrey", 105, 105, 105},
            {"dodgerblue", 30, 144, 255},
            {"firebrick", 178, 34, 34},
            {"floralwhite", 255, 255, 240},
            {"forestgreen", 34, 139, 34},
            {"fuchsia", 255, 0, 255},
            {"gainsboro", 220, 220, 220},
            {"ghostwhite", 248, 248, 255},
            {"gold", 255, 215, 0},
            {"goldenrod", 218, 165, 32},
            {"gray", 128, 128, 128},
            {"green", 0, 128, 0},
            {"greenyellow", 173, 255, 47},
            {"grey", 128, 128, 128},
            {"honeydew", 240, 255, 240},
            {"hotpink", 255, 105, 180},
            {"indianred", 205, 92, 92},
            {"indigo", 75, 0, 130},
            {"ivory", 255, 255, 240},
            {"khaki", 240, 230, 140},
            {"lavender", 230, 230, 250},
            {"lavenderblush", 255, 240, 245},
            {"lawngreen", 124, 252, 0},
            {"lemonchiffon", 255, 250, 205},
            {"lightblue", 173, 216, 230},
            {"lightcoral", 240, 128, 128},
            {"lightcyan", 224, 255, 255},
            {"lightgoldenrodyellow", 250, 250, 210},
            {"lightgray", 211, 211, 211},
            {"lightgreen", 144, 238, 144},
            {"lightgrey", 211, 211, 211},
            {"lightpink", 255, 182, 193},
            {"lightsalmon", 255, 160, 122},
            {"lightseagreen", 32, 178, 170},
            {"lightskyblue", 135, 206, 250},
            {"lightslategray", 119, 136, 153},
            {"lightslategrey", 119, 136, 153},
            {"lightsteelblue", 176, 196, 222},
            {"lightyellow", 255, 255, 224},
            {"lime", 0, 255, 0},
            {"limegreen", 50, 205, 50},
            {"linen", 250, 240, 230},
            {"magenta", 255, 0, 255},
            {"maroon", 128, 0, 0},
            {"mediumaquamarine", 102, 205, 170},
            {"mediumblue", 0, 0, 205},
            {"mediumorchid", 186, 85, 211},
            {"mediumpurple", 147, 112, 219},
            {"mediumseagreen", 60, 179, 113},
            {"mediumslateblue", 123, 104, 238},
            {"mediumspringgreen", 0, 250, 154},
            {"mediumturquoise", 72, 209, 204},
            {"mediumvioletred", 199, 21, 133},
            {"midnightblue", 25, 25, 112},
            {"mintcream", 245, 255, 250},
            {"mistyrose", 255, 228, 225},
            {"moccasin", 255, 228, 181},
            {"navajowhite", 255, 222, 173},
            {"navy", 0, 0, 128},
            {"oldlace", 253, 245, 230},
            {"olive", 128, 128, 0},
            {"olivedrab", 107, 142, 35},
            {"orange", 255, 165, 0},
            {"orangered", 255, 69, 0},
            {"orchid", 218, 112, 214},
            {"palegoldenrod", 238, 232, 170},
            {"palegreen", 152, 251, 152},
            {"paleturquoise", 175, 238, 238},
            {"palevioletred", 219, 112, 147},
            {"papayawhip", 255, 239, 213},
            {"peachpuff", 255, 218, 185},
            {"peru", 205, 133, 63},
            {"pink", 255, 192, 203},
            {"plum", 221, 160, 203},
            {"powderblue", 176, 224, 230},
            {"purple", 128, 0, 128},
            {"red", 255, 0, 0},
            {"rosybrown", 188, 143, 143},
            {"royalblue", 65, 105, 225},
            {"saddlebrown", 139, 69, 19},
            {"salmon", 250, 128, 114},
            {"sandybrown", 244, 164, 96},
            {"seagreen", 46, 139, 87},
            {"seashell", 255, 245, 238},
            {"sienna", 160, 82, 45},
            {"silver", 192, 192, 192},
            {"skyblue", 135, 206, 235},
            {"slateblue", 106, 90, 205},
            {"slategray", 119, 128, 144},
            {"slategrey", 119, 128, 144},
            {"snow", 255, 255, 250},
            {"springgreen", 0, 255, 127},
            {"steelblue", 70, 130, 180},
            {"tan", 210, 180, 140},
            {"teal", 0, 128, 128},
            {"thistle", 216, 191, 216},
            {"tomato", 255, 99, 71},
            {"turquoise", 64, 224, 208},
            {"violet", 238, 130, 238},
            {"wheat", 245, 222, 179},
            {"white", 255, 255, 255},
            {"whitesmoke", 245, 245, 245},
            {"yellow", 255, 255, 0},
            {"yellowgreen", 154, 205, 50}
        };

        if ( const ColorPair *res = std::lower_bound(color_list, color_list + sizeof (color_list) / sizeof (color_list[0]), str, ColorComp() ) ) {
            r = res->r ; g = res->g ; b = res->b ;
            return true ;
        }
        else return false ;
    }

    return false ;
}


Color Color::fromCSS(const string &css_color_spec, float alpha) {
    unsigned char r, g, b ;
    if ( !parse_css_color(css_color_spec, r, g, b) ) {
        stringstream strm ;
        strm << "invalid CSS color: " << css_color_spec ;
        throw CSSColorParseException(strm.str()) ;
    }
    return Color(r/255.0f, g/255.0f, b/255.0f, clamp(alpha)) ;
}

msg::Color *Color::write(const Color &clr) {
    msg::Color *msg_clr = new msg::Color() ;
    msg_clr->set_r(clr.r()) ;
    msg_clr->set_g(clr.g()) ;
    msg_clr->set_b(clr.b()) ;
    msg_clr->set_a(clr.a()) ;
    return msg_clr ;
}

Color Color::read(const msg::Color &clr) {
    return Color(clr.r(), clr.g(), clr.b(), clr.a());
}

Color Color::alice_blue() noexcept { return RGB{240, 248, 255} ; }
Color Color::antique_white() noexcept { return RGB{250, 235, 215} ; }
Color Color::aqua() noexcept { return RGB{0, 255, 255} ; }
Color Color::aqua_marine() noexcept { return RGB{127, 255, 212} ; }
Color Color::azure() noexcept { return RGB{240, 255, 255} ; }
Color Color::beige() noexcept { return RGB{245, 245, 220} ; }
Color Color::bisque() noexcept { return RGB{255, 228, 196} ; }
Color Color::black() noexcept { return RGB{0, 0, 0} ; }
Color Color::blanched_almond() noexcept { return RGB{255, 235, 205} ; }
Color Color::blue() noexcept { return RGB{0, 0, 255} ; }
Color Color::blue_violet() noexcept { return RGB{138, 43, 226} ; }
Color Color::brown() noexcept { return RGB{165, 42, 42} ; }
Color Color::burlywood() noexcept { return RGB{222, 184, 135} ; }
Color Color::cadet_blue() noexcept { return RGB{95, 158, 160} ; }
Color Color::chartreuse() noexcept { return RGB{127, 255, 0} ; }
Color Color::chocolate() noexcept { return RGB{210, 105, 30} ; }
Color Color::coral() noexcept { return RGB{255, 127, 80} ; }
Color Color::cornflower_blue() noexcept { return RGB{100, 149, 237} ; }
Color Color::corn_silk() noexcept { return RGB{255, 248, 220} ; }
Color Color::crimson() noexcept { return RGB{220, 20, 60} ; }
Color Color::cyan() noexcept { return RGB{0, 255, 255} ; }
Color Color::dark_blue() noexcept { return RGB{0, 0, 139} ; }
Color Color::dark_cyan() noexcept { return RGB{0, 139, 139} ; }
Color Color::dark_golden_rod() noexcept { return RGB{184, 132, 11} ; }
Color Color::dark_gray() noexcept { return RGB{169, 169, 169} ; }
Color Color::dark_green() noexcept { return RGB{0, 100, 0} ; }
Color Color::dark_grey() noexcept { return RGB{169, 169, 169} ; }
Color Color::dark_khaki() noexcept { return RGB{189, 183, 107} ; }
Color Color::dark_magenta() noexcept { return RGB{139, 0, 139} ; }
Color Color::dark_olive_green() noexcept { return RGB{85, 107, 47} ; }
Color Color::dark_orange() noexcept { return RGB{255, 140, 0} ; }
Color Color::dark_orchid() noexcept { return RGB{153, 50, 204} ; }
Color Color::dark_red() noexcept { return RGB{139, 0, 0} ; }
Color Color::dark_salmon() noexcept { return RGB{233, 150, 122} ; }
Color Color::dark_sea_green() noexcept { return RGB{143, 188, 143} ; }
Color Color::dark_slate_blue() noexcept { return RGB{72, 61, 139} ; }
Color Color::dark_slate_gray() noexcept { return RGB{47, 79, 79} ; }
Color Color::dark_slate_grey() noexcept { return RGB{47, 79, 79} ; }
Color Color::dark_turquoise() noexcept { return RGB{0, 206, 209} ; }
Color Color::dark_violet() noexcept { return RGB{148, 0, 211} ; }
Color Color::deep_pink() noexcept { return RGB{255, 20, 147} ; }
Color Color::deep_sky_blue() noexcept { return RGB{0, 191, 255} ; }
Color Color::dim_gray() noexcept { return RGB{105, 105, 105} ; }
Color Color::dim_grey() noexcept { return RGB{105, 105, 105} ; }
Color Color::dodger_blue() noexcept { return RGB{30, 144, 255} ; }
Color Color::fire_brick() noexcept { return RGB{178, 34, 34} ; }
Color Color::floral_white() noexcept { return RGB{255, 255, 240} ; }
Color Color::forest_green() noexcept { return RGB{34, 139, 34} ; }
Color Color::fuchsia() noexcept { return RGB{255, 0, 255} ; }
Color Color::gainsboro() noexcept { return RGB{220, 220, 220} ; }
Color Color::ghostwhite() noexcept { return RGB{248, 248, 255} ; }
Color Color::gold() noexcept { return RGB{255, 215, 0} ; }
Color Color::goldenrod() noexcept { return RGB{218, 165, 32} ; }
Color Color::gray() noexcept { return RGB{128, 128, 128} ; }
Color Color::green() noexcept { return RGB{0, 128, 0} ; }
Color Color::green_yellow() noexcept { return RGB{173, 255, 47} ; }
Color Color::grey() noexcept { return RGB{128, 128, 128} ; }
Color Color::honey_dew() noexcept { return RGB{240, 255, 240} ; }
Color Color::hot_pink() noexcept { return RGB{255, 105, 180} ; }
Color Color::indian_red() noexcept { return RGB{205, 92, 92} ; }
Color Color::indigo() noexcept { return RGB{75, 0, 130} ; }
Color Color::ivory() noexcept { return RGB{255, 255, 240} ; }
Color Color::khaki() noexcept { return RGB{240, 230, 140} ; }
Color Color::lavender() noexcept { return RGB{230, 230, 250} ; }
Color Color::lavender_blush() noexcept { return RGB{255, 240, 245} ; }
Color Color::lawn_green() noexcept { return RGB{124, 252, 0} ; }
Color Color::lemon_chiffon() noexcept { return RGB{255, 250, 205} ; }
Color Color::light_blue() noexcept { return RGB{173, 216, 230} ; }
Color Color::light_coral() noexcept { return RGB{240, 128, 128} ; }
Color Color::light_cyan() noexcept { return RGB{224, 255, 255} ; }
Color Color::light_golden_rod_yellow() noexcept { return RGB{250, 250, 210} ; }
Color Color::light_gray() noexcept { return RGB{211, 211, 211} ; }
Color Color::light_green() noexcept { return RGB{144, 238, 144} ; }
Color Color::light_grey() noexcept { return RGB{211, 211, 211} ; }
Color Color::light_pink() noexcept { return RGB{255, 182, 193} ; }
Color Color::light_salmon() noexcept { return RGB{255, 160, 122} ; }
Color Color::light_sea_green() noexcept { return RGB{32, 178, 170} ; }
Color Color::light_sky_blue() noexcept { return RGB{135, 206, 250} ; }
Color Color::light_slate_gray() noexcept { return RGB{119, 136, 153} ; }
Color Color::light_slate_grey() noexcept { return RGB{119, 136, 153} ; }
Color Color::light_steel_blue() noexcept { return RGB{176, 196, 222} ; }
Color Color::light_yellow() noexcept { return RGB{255, 255, 224} ; }
Color Color::lime() noexcept { return RGB{0, 255, 0} ; }
Color Color::limegreen() noexcept { return RGB{50, 205, 50} ; }
Color Color::linen() noexcept { return RGB{250, 240, 230} ; }
Color Color::magenta() noexcept { return RGB{255, 0, 255} ; }
Color Color::maroon() noexcept { return RGB{128, 0, 0} ; }
Color Color::medium_aqua_marine() noexcept { return RGB{102, 205, 170} ; }
Color Color::medium_blue() noexcept { return RGB{0, 0, 205} ; }
Color Color::medium_orchid() noexcept { return RGB{186, 85, 211} ; }
Color Color::medium_purple() noexcept { return RGB{147, 112, 219} ; }
Color Color::medium_sea_green() noexcept { return RGB{60, 179, 113} ; }
Color Color::medium_slate_blue() noexcept { return RGB{123, 104, 238} ; }
Color Color::medium_spring_green() noexcept { return RGB{0, 250, 154} ; }
Color Color::medium_turquoise() noexcept { return RGB{72, 209, 204} ; }
Color Color::medium_violet_red() noexcept { return RGB{199, 21, 133} ; }
Color Color::midnight_blue() noexcept { return RGB{25, 25, 112} ; }
Color Color::mint_cream() noexcept { return RGB{245, 255, 250} ; }
Color Color::misty_rose() noexcept { return RGB{255, 228, 225} ; }
Color Color::moccasin() noexcept { return RGB{255, 228, 181} ; }
Color Color::navajo_white() noexcept { return RGB{255, 222, 173} ; }
Color Color::navy() noexcept { return RGB{0, 0, 128} ; }
Color Color::old_lace() noexcept { return RGB{253, 245, 230} ; }
Color Color::olive() noexcept { return RGB{128, 128, 0} ; }
Color Color::olive_drab() noexcept { return RGB{107, 142, 35} ; }
Color Color::orange() noexcept { return RGB{255, 165, 0} ; }
Color Color::orangered() noexcept { return RGB{255, 69, 0} ; }
Color Color::orchid() noexcept { return RGB{218, 112, 214} ; }
Color Color::pale_golden_rod() noexcept { return RGB{238, 232, 170} ; }
Color Color::pale_green() noexcept { return RGB{152, 251, 152} ; }
Color Color::pale_turquoise() noexcept { return RGB{175, 238, 238} ; }
Color Color::pale_violet_red() noexcept { return RGB{219, 112, 147} ; }
Color Color::papaya_whip() noexcept { return RGB{255, 239, 213} ; }
Color Color::peach_puff() noexcept { return RGB{255, 218, 185} ; }
Color Color::peru() noexcept { return RGB{205, 133, 63} ; }
Color Color::pink() noexcept { return RGB{255, 192, 203} ; }
Color Color::plum() noexcept { return RGB{221, 160, 203} ; }
Color Color::powderblue() noexcept { return RGB{176, 224, 230} ; }
Color Color::purple() noexcept { return RGB{128, 0, 128} ; }
Color Color::red() noexcept { return RGB{255, 0, 0} ; }
Color Color::rosy_brown() noexcept { return RGB{188, 143, 143} ; }
Color Color::royal_blue() noexcept { return RGB{65, 105, 225} ; }
Color Color::saddle_brown() noexcept { return RGB{139, 69, 19} ; }
Color Color::salmon() noexcept { return RGB{250, 128, 114} ; }
Color Color::sandybrown() noexcept { return RGB{244, 164, 96} ; }
Color Color::seagreen() noexcept { return RGB{46, 139, 87} ; }
Color Color::seashell() noexcept { return RGB{255, 245, 238} ; }
Color Color::sienna() noexcept { return RGB{160, 82, 45} ; }
Color Color::silver() noexcept { return RGB{192, 192, 192} ; }
Color Color::sky_blue() noexcept { return RGB{135, 206, 235} ; }
Color Color::slate_blue() noexcept { return RGB{106, 90, 205} ; }
Color Color::slate_gray() noexcept { return RGB{119, 128, 144} ; }
Color Color::slate_grey() noexcept { return RGB{119, 128, 144} ; }
Color Color::snow() noexcept { return RGB{255, 255, 250} ; }
Color Color::spring_green() noexcept { return RGB{0, 255, 127} ; }
Color Color::steel_blue() noexcept { return RGB{70, 130, 180} ; }
Color Color::tan() noexcept { return RGB{210, 180, 140} ; }
Color Color::teal() noexcept { return RGB{0, 128, 128} ; }
Color Color::thistle() noexcept { return RGB{216, 191, 216} ; }
Color Color::tomato() noexcept { return RGB{255, 99, 71} ; }
Color Color::turquoise() noexcept { return RGB{64, 224, 208} ; }
Color Color::violet() noexcept { return RGB{238, 130, 238} ; }
Color Color::wheat() noexcept { return RGB{245, 222, 179} ; }
Color Color::white() noexcept { return RGB{255, 255, 255} ; }
Color Color::white_smoke() noexcept { return RGB{245, 245, 245} ; }
Color Color::yellow() noexcept { return RGB{255, 255, 0} ; }
Color Color::yellow_green() noexcept { return RGB{154, 205, 50} ; }

double Color::clamp(double value) {
    return std::max(0.0, std::min(1.0, value));
}

}











