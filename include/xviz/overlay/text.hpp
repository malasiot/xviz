#pragma once

#include <xviz/overlay/overlay.hpp>
#include <xviz/common/font.hpp>

#include <Eigen/Geometry>

namespace xviz {

namespace impl {
class OpenGLText ;
}


class Text: public OverlayItem {
public:
    enum Anchor { AnchorLeft, AnchorRight, AnchorMiddle } ;

    Text(const std::string &str, const Font &font, const Eigen::Vector3f &clr, Anchor anchor = AnchorLeft) ;
    ~Text() ;

    float width() const ;
    float height() const  ;

    float ascent() const ;
    float descent() const ;
    float advance() const ;
    float leading() const;

    void draw() override ;

    void updateText(const std::string &text) ;

private:

    std::unique_ptr<impl::OpenGLText> text_ ;
    Anchor anchor_ ;
    Font font_ ;
    Eigen::Vector3f clr_ ;
};

class TextBox: public OverlayContainer {
public:

    enum TextAlignFlags {
        AlignLeft = 0x01, AlignRight = 0x02, AlignTop = 0x04, AlignBottom = 0x08, AlignHCenter = 0x10, AlignVCenter = 0x20, AlignBaseline = 0x40
    }  ;

    TextBox() = default;
    ~TextBox() ;

    void setAlignment(uint flags) ;

    void addTextSpan(Text *item) ;
    void newLine() ;

    void layout() override ;

    void draw() override ;

private:

    void measure(float &mw, float &mh) override ;

    float line_spacing_ = 0;
    uint align_ = AlignLeft | AlignTop ;

    struct Line {
        float offset_, width_, height_, ascent_, descent_, leading_ ;
        std::vector<Text *> spans_ ;
    };

    std::vector<std::unique_ptr<Text>> items_ ;
    std::vector<Line> lines_ ;
    float content_width_, content_height_ ;

};

}
