#include <xviz/overlay/text.hpp>
#include "text_item.hpp"
#include "gl_rect.hpp"

namespace xviz {

Text::Text(const std::string &str, const Font &font, const Eigen::Vector3f &clr, Anchor a): font_(font), clr_(clr), anchor_(a) {
    text_.reset(new impl::OpenGLText(str, font, clr)) ;
}

Text::~Text() {
}

float Text::width() const {
    return text_->line().width() ;
}

float Text::height() const {
    return text_->line().height() ;
}

float Text::advance() const {
    return text_->line().advance() ;
}

float Text::descent() const {
    return text_->line().descent() ;
}

float Text::ascent() const {
    return text_->line().ascent() ;
}

float Text::leading() const {
    return text_->line().leading() ;
}

void Text::draw() {
    float offset = 0 ;
    if ( anchor_ == AnchorRight )
        offset = width() ;
    else if ( anchor_ == AnchorMiddle )
        offset = width()/2.0 ;

    text_->render(x_ - offset, y_) ;
}

void Text::updateText(const std::string &text) {
    text_.reset(new impl::OpenGLText(text, font_, clr_)) ;
    if ( parent_ ) parent_->updateLayout();
}


TextBox::~TextBox()
{

}

void TextBox::setAlignment(uint flags) {
    align_ = flags ;
}

void TextBox::addTextSpan(Text *item) {
    items_.emplace_back(item) ;
    if ( lines_.empty() ) lines_.emplace_back() ;
    lines_.back().spans_.emplace_back(item) ;
}

void TextBox::newLine() {
    lines_.emplace_back() ;
}

void TextBox::layout() {
    double y = 0, tx = 0, ty = 0 ;

    if ( align_ & AlignVCenter )
        ty = ( h_ - content_height_)/2;
    else if ( align_ & AlignBottom )
        ty = h_ - content_height_  ;

    bool is_first_line = true ;

    y = y_ + ty + margins_.top_ ;

    for (const Line &line: lines_ ) {
        if ( align_ & AlignHCenter )
            tx =  ( w_ - line.width_ - margins_.left_ - margins_.right_)/2.0 ;
        else if ( align_ & AlignRight )
            tx =  w_ - line.width_ - margins_.left_ - margins_.right_ ;

        if ( is_first_line ) {
            y += line.ascent_ ;
            is_first_line = false ;
        } else {
            y += line.leading_ ;
        }

        float x = x_ + tx + margins_.left_;
        for( size_t i = 0 ; i<line.spans_.size() ; i++ ) {
            Text *ts = line.spans_[i] ;
            ts->setPosition(x, y) ;
            x += ts->width() ;
            if ( i < line.spans_.size() - 1 )
                x += ts->advance() ;
        }
    }

}


void TextBox::draw() {
    for( const auto &c: items_ )
        c->draw() ;
}

void TextBox::measure(float &mw, float &mh) {
    if ( lines_.empty() ) return ;
    // compute line metrics

    for( auto &l: lines_ ) {
        l.width_ = 0 ; l.height_ = 0 ; l.ascent_ = 0 ; l.descent_ = 0 ;
        for( size_t i=0 ; i<l.spans_.size() ; i++ ) {
            const auto &span = l.spans_[i] ;
            float w = ( i < l.spans_.size() - 1 ) ? span->width() + span->advance() : span->width() ;
            l.width_ += w ;
            l.height_ = std::max(l.height_, span->height()) ;
            l.ascent_ = std::max(l.ascent_, span->ascent()) ;
            l.descent_ = std::min(l.descent_, span->descent()) ;
            l.leading_ = std::max(l.leading_, span->leading()) ;
        }
    }

    // get content height

    content_height_ = lines_[0].ascent_ + margins_.top_  ;
    content_width_ = 0;

    uint i ;
    for( i=0 ; i<lines_.size() - 1; i++ ) {
        const Line &l = lines_[i] ;
        content_height_ += l.leading_ ;
        content_width_ = std::max(content_width_, l.width_) ;
    }

    content_height_ += -lines_[i].descent_ ;
    content_height_ += margins_.bottom_ ;
    content_width_ = std::max(content_width_, lines_[i].width_) ;

    content_width_ += margins_.left_ ;
    content_width_ += margins_.right_ ;

    mw = content_width_ ;
    mh = content_height_ ;
}

}
