#pragma once

#include <vector>
#include <memory>
#include <Eigen/Core>

namespace xviz {

struct Margins {
    Margins() = default ;
    Margins(float l, float t, float r, float b): left_(l), top_(t), right_(t), bottom_(b) {}

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


class OverlayItem {
public:
    OverlayItem() = default ;

    void setPosition(float x, float y) ;

    float x() const { return x_ ; }
    float y() const { return y_ ; }

    virtual void draw() =0 ;

protected:
    float x_, y_ ;

};

enum MeasureMode {
    MeasureModeExact, MeasureModeAtMost, MeasureModeUndefined
};

class OverlayContainer: public OverlayItem {
public:
    OverlayContainer() = default ;
    void setMargins(float left, float top, float right, float bottom) ;

    virtual void setSize(const Length &w, const Length &h) ;

    virtual std::pair<float, float> measure(float w, MeasureMode m_horz, float h, MeasureMode m_vert) ;

    virtual void draw() {}

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

    void setParent(OverlayContainer *p) { parent_ = p ; }

protected:

    float w_,  h_ ;
    Margins margins_ ;
    std::optional<float> min_width_, min_height_ ;
    std::optional<float> max_width_, max_height_ ;
    float stretch_ = 1.0 ;
    OverlayContainer *parent_ = nullptr ;
};

class Frame: public OverlayContainer {
public:
    Frame(OverlayContainer *child = nullptr) ;

    void setBorderWidth(float bw) { bw_ = bw ; }
    void setBorderColor(const Eigen::Vector3f &clr) { stroke_ = clr ; }
    void setBackgroundColor(const Eigen::Vector3f &fill) { fill_ = fill ;}
    void setOpacity(float a) { opacity_ = a; }

    void layout() override ;
protected:

    void draw() ;

    float bw_ = 1;
    Eigen::Vector3f fill_ = {1, 1, 1}, stroke_ = {0, 0, 0} ;
    float opacity_ = 1 ;
    std::unique_ptr<OverlayContainer> child_ ;
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
