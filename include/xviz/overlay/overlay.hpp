#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <Eigen/Core>

namespace xviz {

struct Margins {
    Margins() = default ;
    Margins(float l, float t, float r, float b): left_(l), top_(t), right_(r), bottom_(b) {}

    float left_ = 0.f, top_ = 0.f, right_ = 0.f, bottom_ = 0.f;
};


class Length {
public:
    enum Units { Pixels, Percentage, Undefined } ;

    Length() = default ;
    Length(float val, Units units = Units::Pixels): units_(units), value_(val) {  }

    float value() const { return value_ ; }
    Units units() const { return units_ ; }

private:
    Units units_ ;
    float value_ ;
};

inline Length operator "" _perc( long double v ) {
    return Length{static_cast<float>(v), Length::Percentage};
}

inline Length operator "" _px( long double v ) {
    return Length{ static_cast<float>(v), Length::Pixels };
}

class OverlayContainer ;

class OverlayItem {
public:
    OverlayItem() = default ;

    void setPosition(float x, float y) ;

    float x() const { return x_ ; }
    float y() const { return y_ ; }

    virtual void draw() =0 ;

    void setParent(OverlayContainer *p) { parent_ = p ; }


protected:
    float x_, y_ ;
    OverlayContainer *parent_ = nullptr ;

};

enum MeasureMode {
    MeasureModeExact, MeasureModeAtMost, MeasureModeUndefined
};

struct MeasureSpec {
    MeasureSpec(MeasureMode mode, float dim): mode_(mode), dim_(dim) {}

    MeasureMode mode_ ;
    float dim_ ;
};

class OverlayContainer: public OverlayItem {
public:
    OverlayContainer() = default ;
    void setMargins(float left, float top, float right, float bottom) ;

    virtual void setSize(const Length &w, const Length &h) ;

    virtual void layout() {}

    float width() const { return w_ ; }
    float height() const { return h_ ; }

    void setMinWidth(float w) { min_width_ = w ; }
    void setMinHeight(float h) { min_height_ = h ; }

    void setMaxWidth(float w) { max_width_ = w ; }
    void setMaxHeight(float h) { max_height_ = h ; }

    void setStretch(float stretch) { stretch_ = stretch ; }

    const auto &minWidth() const { return min_width_ ; }
    const auto &minHeight() const { return min_height_ ; }
    const auto &maxWidth() const { return max_width_ ; }
    const auto &maxHeight() const { return max_height_ ; }

    float stretch() const { return stretch_ ; }

    virtual void measure(float &mw, float &mh) = 0;
    virtual void draw() {}
    void updateLayout() ;

protected:

    float w_,  h_ ;
    Margins margins_ ;
    std::optional<float> min_width_, min_height_ ;
    std::optional<float> max_width_, max_height_ ;
    float stretch_ = 1.0 ;
};



class OverlayGroup: public OverlayItem {
public:
    void addChild(OverlayItem *item) {
        children_.emplace_back(item) ;
    }

    virtual void layoutChildren() {}

    void draw() override  {
        for( const auto &c: children_ )
            c->draw() ;
    }


private:
    std::vector<std::unique_ptr<OverlayItem>> children_ ;
};


}
