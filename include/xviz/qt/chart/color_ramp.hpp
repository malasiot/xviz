#ifndef XVIZ_QT_COLOR_RAMP_HPP
#define XVIZ_QT_COLOR_RAMP_HPP

#include <xviz/colormap.hpp>

#include <QRect>
#include <QPainter>

#include "axis.hpp"

class ColorRamp {
public:
    ColorRamp(int type, float vmin, float vmax): cmap_(type), vmin_(vmin), vmax_(vmax) {
            axis_.setRange(vmin, vmax) ;
            axis_.setTickPlacement(Axis::TicsInside) ;
    }

    // compute bounding box
    QRect layout(float h) ;

    void paint(QPainter &p, const QRect &box) ;

private:

    QImage renderBar(const QSize &rect) ;

    const float fraction_ = 1.0f ;
    const float width_ = 20.f ;
    const float pad_ = 10.f ;
    const float label_offset_ = 6.0f ;
    const float tic_size_ = 4.f ;

    int cmap_ ;
    float vmin_, vmax_ ;

    QFont label_font_ ;

    YAxis axis_ ;


};

#endif
