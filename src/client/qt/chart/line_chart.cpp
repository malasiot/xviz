#include <xviz/qt/chart/line_chart.hpp>

#include "../qt_graphics_helpers.hpp"
#include <limits>
#include <QPainterPath>

using namespace std ;
/*
void QLineChart::paint(QPainter &c, const QRect &rect) {

    c.setPen(Qt::black) ;

    qreal w = rect.width() ;
    qreal h = rect.height() ;
    qreal title_height = 0;

    // compute axis layout to determine valid plot rectangle

    if ( !title_.isEmpty() ) {
        QFontMetrics fm(title_font_) ;
        QRect layout(0, 0, max_title_width_, 100 ) ;
        QRect br = fm.boundingRect(layout, Qt::AlignHCenter|Qt::AlignTop|Qt::TextWordWrap, title_) ;
        h -= title_height = br.height() ;
    }

    x_axis_.computeLayout(w) ;
    y_axis_.computeLayout(h) ;

    qreal ox = y_axis_.size() ;
    qreal oy = x_axis_.size() ;

    w -= ox ;
    h -= oy ;

    x_axis_.computeTransform(w) ;
    y_axis_.computeTransform(h) ;

    // draw

    c.save() ;
    c.translate(ox, -oy) ;

    // background

    c.fillRect(0, 0, w, -h, bg_brush_) ;

    // x-y axis

    x_axis_.draw(c, w, h);
    y_axis_.draw(c, w, h);

    // opposite lines

    c.drawLine(0, -h, w, -h) ;
    c.drawLine(w, 0, w, -h) ;

    // data

    c.save();
    c.setClipRect(0, 0, w, -h) ;

    for( uint i=0 ; i<entries_.size() ; i++ ) {
        const auto &ls = chart_->series()[i] ;
        const auto &e = entries_[i] ;
        if ( e.flags_ & LegendEntry::HasPen )
            paintLine(c, e.pen_, ls) ;
        if ( e.flags_ & LegendEntry::HasMarker )
            paintMarkers(c, e.marker_, ls) ;

        const auto &errors = ls.e() ;
        if ( !errors.empty() ) {
            paintErrorBars(c, ls) ;
        }

    }

    c.restore() ;

    // legend

    legend_.draw(c, entries_, w, h);


    if ( !title_.isEmpty() ) {
        c.save() ;
        QRect layout(w/2- max_title_width_/2, -h - title_height - title_offset_, max_title_width_, 100 ) ;
        c.setFont(title_font_) ;
        c.drawText(layout, Qt::AlignHCenter|Qt::AlignTop|Qt::TextWordWrap, title_) ;
        c.restore() ;
    }

    c.restore() ;
}
*/

void QLineChart::paintChart(QPainter &c, const QSize &) {
    for( uint i=0 ; i<entries_.size() ; i++ ) {
        const auto &ls = chart()->series()[i] ;
        const auto &e = entries_[i] ;
        if ( e.flags_ & LegendEntry::HasPen )
            paintLine(c, e.pen_, ls) ;
        if ( e.flags_ & LegendEntry::HasMarker )
            paintMarkers(c, e.marker_, ls) ;

        const auto &errors = ls.e() ;
        if ( !errors.empty() ) {
            paintErrorBars(c, ls) ;
        }

    }
}

void QLineChart::paintErrorBars(QPainter &c, const xviz::LineSeries &ls) {
    const auto &x = ls.x() ;
    const auto &y = ls.y() ;
    const auto &e = ls.e() ;

    for( size_t i=0 ; i<x.size() ; i += draw_error_bars_every_nth_ ) {
        double xp = x_axis_.transform(x[i]) ;
        double ep =  e[i];

        double y1 = y_axis_.transform(y[i] - ep) ;
        double y2 = y_axis_.transform(y[i] + ep) ;

        c.save() ;
        c.setPen(error_bar_pen_) ;
        c.drawLine(xp, y1, xp, y2) ;
        c.restore() ;

        c.save() ;
        c.setPen(error_bar_cap_pen_) ;
        c.drawLine(xp -error_bar_cap_width_/2, y1, xp+error_bar_cap_width_/2, y1) ;
        c.drawLine(xp -error_bar_cap_width_/2, y2, xp+error_bar_cap_width_/2, y2) ;
        c.restore() ;
    }
}

void QLineChart::paintLine(QPainter &c, const QPen &pen, const xviz::LineSeries &ls) {
    QPainterPath p ;

    const auto &vx = ls.x() ;
    const auto &vy = ls.y() ;

    for( uint i=0 ; i<vx.size() ; i++ ) {
        qreal x = x_axis_.transform(vx[i]) ;
        qreal y = y_axis_.transform(vy[i]) ;

        if ( i == 0 ) p.moveTo(x, y) ;
        else p.lineTo(x, y) ;
    }

    c.save() ;
    c.setPen(pen) ;
    c.drawPath(p) ;
    c.restore() ;
}

void QLineChart::paintMarkers(QPainter &c, const QPixmap &marker, const xviz::LineSeries &ls) {

    const auto &vx = ls.x() ;
    const auto &vy = ls.y() ;

    for( uint i=0 ; i<vx.size() ; i++ ) {
        qreal x = x_axis_.transform(vx[i]) ;
        qreal y = y_axis_.transform(vy[i]) ;

        c.drawPixmap(x-marker.width()/2, y-marker.height()/2, marker) ;
    }
}

QRectF QLineChart::getDataBounds()
{
    qreal minx = numeric_limits<qreal>::max(),
            miny = numeric_limits<qreal>::max() ,
            maxx = -numeric_limits<qreal>::max(),
            maxy = -numeric_limits<qreal>::max() ;

    for( const xviz::LineSeries &ls: chart_->series() ) {
        const auto &x = ls.x() ;
        const auto &y = ls.y() ;

        for( size_t i=0 ; i<x.size() ; i++ ) {
            minx = std::min(minx, x[i]) ;
            miny = std::min(miny, y[i]) ;
            maxx = std::max(maxx, x[i]) ;
            maxy = std::max(maxy, y[i]) ;
        }
    }

    return QRectF(minx, miny, maxx - minx, maxy - miny) ;
}

QLineChart::QLineChart(const xviz::LineChart *lc): Chart(lc) {
}

void QLineChart::makeLegendEntries() {
    const xviz::LineChart *lc = static_cast<const xviz::LineChart *>(chart_.get()) ;

    for( const xviz::LineSeries &ls: lc->series()) {
        LegendEntry e ;
        e.flags_ = 0 ;
        e.label_ = QString::fromStdString(ls.title()) ;
        e.pen_ = qPenFromSolidPen(ls.pen()) ;
        e.marker_ = qPixmapFromMarker(ls.marker());

        entries_.append(e) ;
    }

}
