#ifndef XVIZ_COLOR_HPP
#define XVIZ_COLOR_HPP
#include <string>
#include <stdexcept>

namespace xviz {

// RGBA color container
// Components are always clamped in the range [0, 1]

namespace msg {
    class Color ;
}

struct RGB {
    RGB(unsigned char r, unsigned char g, unsigned char b): r_(r), g_(g), b_(b) {}
    unsigned char r_, g_, b_ ;
};

class Color
{
public:
    Color() = default ;

    Color(float r, float g, float b, float alpha = 1.0):
        r_(clamp(r)), g_(clamp(g)), b_(clamp(b)), a_(clamp(alpha)) {}

    Color(const RGB &rgb): r_(rgb.r_/255.f), g_(rgb.g_/255.f), b_(rgb.b_/255.f) {}

    static Color fromCSS(const std::string &css_color, float alpha = 1.0) ;

    float r() const noexcept { return r_ ; }
    float g() const noexcept { return g_ ; }
    float b() const noexcept { return b_ ; }
    float a() const noexcept { return a_ ; }

    void setAlpha(float alpha) noexcept { a_ = clamp(alpha) ; }
    void setRed(float red) noexcept { r_ = clamp(red) ; }
    void setGreen(float green) noexcept { g_ = clamp(green) ; }
    void setBlue(float blue) noexcept { b_ = clamp(blue) ; }

    static Color read(const msg::Color &) ;
    static msg::Color *write(const Color &clr) ;

    static Color alice_blue() noexcept;
    static Color antique_white() noexcept;
    static Color aqua() noexcept;
    static Color aqua_marine() noexcept;
    static Color azure() noexcept;
    static Color beige() noexcept;
    static Color bisque() noexcept;
    static Color black() noexcept;
    static Color blanched_almond() noexcept;
    static Color blue() noexcept;
    static Color blue_violet() noexcept;
    static Color brown() noexcept;
    static Color burlywood() noexcept;
    static Color cadet_blue() noexcept;
    static Color chartreuse() noexcept;
    static Color chocolate() noexcept;
    static Color coral() noexcept;
    static Color cornflower_blue() noexcept;
    static Color corn_silk() noexcept;
    static Color crimson() noexcept;
    static Color cyan() noexcept;
    static Color dark_blue() noexcept;
    static Color dark_cyan() noexcept;
    static Color dark_golden_rod() noexcept;
    static Color dark_gray() noexcept;
    static Color dark_green() noexcept;
    static Color dark_grey() noexcept;
    static Color dark_khaki() noexcept;
    static Color dark_magenta() noexcept;
    static Color dark_olive_green() noexcept;
    static Color dark_orange() noexcept;
    static Color dark_orchid() noexcept;
    static Color dark_red() noexcept;
    static Color dark_salmon() noexcept;
    static Color dark_sea_green() noexcept;
    static Color dark_slate_blue() noexcept;
    static Color dark_slate_gray() noexcept;
    static Color dark_slate_grey() noexcept;
    static Color dark_turquoise() noexcept;
    static Color dark_violet() noexcept;
    static Color deep_pink() noexcept;
    static Color deep_sky_blue() noexcept;
    static Color dim_gray() noexcept;
    static Color dim_grey() noexcept;
    static Color dodger_blue() noexcept;
    static Color fire_brick() noexcept;
    static Color floral_white() noexcept;
    static Color forest_green() noexcept;
    static Color fuchsia() noexcept;
    static Color gainsboro() noexcept;
    static Color ghostwhite() noexcept;
    static Color gold() noexcept;
    static Color goldenrod() noexcept;
    static Color gray() noexcept;
    static Color green() noexcept;
    static Color green_yellow() noexcept;
    static Color grey() noexcept;
    static Color honey_dew() noexcept;
    static Color hot_pink() noexcept;
    static Color indian_red() noexcept;
    static Color indigo() noexcept;
    static Color ivory() noexcept;
    static Color khaki() noexcept;
    static Color lavender() noexcept;
    static Color lavender_blush() noexcept;
    static Color lawn_green() noexcept;
    static Color lemon_chiffon() noexcept;
    static Color light_blue() noexcept;
    static Color light_coral() noexcept;
    static Color light_cyan() noexcept;
    static Color light_golden_rod_yellow() noexcept;
    static Color light_gray() noexcept;
    static Color light_green() noexcept;
    static Color light_grey() noexcept;
    static Color light_pink() noexcept;
    static Color light_salmon() noexcept;
    static Color light_sea_green() noexcept;
    static Color light_sky_blue() noexcept;
    static Color light_slate_gray() noexcept;
    static Color light_slate_grey() noexcept;
    static Color light_steel_blue() noexcept;
    static Color light_yellow() noexcept;
    static Color lime() noexcept;
    static Color limegreen() noexcept;
    static Color linen() noexcept;
    static Color magenta() noexcept;
    static Color maroon() noexcept;
    static Color medium_aqua_marine() noexcept;
    static Color medium_blue() noexcept;
    static Color medium_orchid() noexcept;
    static Color medium_purple() noexcept;
    static Color medium_sea_green() noexcept;
    static Color medium_slate_blue() noexcept;
    static Color medium_spring_green() noexcept;
    static Color medium_turquoise() noexcept;
    static Color medium_violet_red() noexcept;
    static Color midnight_blue() noexcept;
    static Color mint_cream() noexcept;
    static Color misty_rose() noexcept;
    static Color moccasin() noexcept;
    static Color navajo_white() noexcept;
    static Color navy() noexcept;
    static Color old_lace() noexcept;
    static Color olive() noexcept;
    static Color olive_drab() noexcept;
    static Color orange() noexcept;
    static Color orangered() noexcept;
    static Color orchid() noexcept;
    static Color pale_golden_rod() noexcept;
    static Color pale_green() noexcept;
    static Color pale_turquoise() noexcept;
    static Color pale_violet_red() noexcept;
    static Color papaya_whip() noexcept;
    static Color peach_puff() noexcept;
    static Color peru() noexcept;
    static Color pink() noexcept;
    static Color plum() noexcept;
    static Color powderblue() noexcept;
    static Color purple() noexcept;
    static Color red() noexcept;
    static Color rosy_brown() noexcept;
    static Color royal_blue() noexcept;
    static Color saddle_brown() noexcept;
    static Color salmon() noexcept;
    static Color sandybrown() noexcept;
    static Color seagreen() noexcept;
    static Color seashell() noexcept;
    static Color sienna() noexcept;
    static Color silver() noexcept;
    static Color sky_blue() noexcept;
    static Color slate_blue() noexcept;
    static Color slate_gray() noexcept;
    static Color slate_grey() noexcept;
    static Color snow() noexcept;
    static Color spring_green() noexcept;
    static Color steel_blue() noexcept;
    static Color tan() noexcept;
    static Color teal() noexcept;
    static Color thistle() noexcept;
    static Color tomato() noexcept;
    static Color turquoise() noexcept;
    static Color violet() noexcept;
    static Color wheat() noexcept;
    static Color white() noexcept;
    static Color white_smoke() noexcept;
    static Color yellow() noexcept;
    static Color yellow_green() noexcept;

private:

    static double clamp(double value) ;

    float r_= 0.f, g_ = 0.f, b_ = 0.f, a_ = 1.0f ;
} ;

class CSSColorParseException: public std::runtime_error {
public:
    CSSColorParseException(const std::string &msg): std::runtime_error(msg) {}
} ;

} // xviz

#endif
