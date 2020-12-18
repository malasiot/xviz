#include <xviz/qt/chart/bar_chart.hpp>

#include "../qt_graphics_helpers.hpp"
#include <limits>
#include <QPainterPath>

using namespace std ;

void QBarChart::paintChart(QPainter &c, const QSize &) {
    const xviz::BarChart *lc = dynamic_cast<const xviz::BarChart *>(chart_.get()) ;
    for( uint i=0 ; i<entries_.size() ; i++ ) {
        const auto &ls = lc->series()[i] ;
        const auto &e = entries_[i] ;

        paintBars(c, e.pen_, e.brush_, ls) ;
    }
}

void QBarChart::paintBars(QPainter &c, const QPen &pen, const QBrush &brush, const xviz::BarSeries &ls) {
    const auto &x = ls.x() ;
    const auto &height = ls.height() ;
    const auto &bottom = ls.bottom() ;

    double width = ls.width() ;

    for( size_t i=0 ; i<x.size() ; i++ ) {
        double x0 = x_axis_.transform(x[i]-width/2.0) ;
        double x1 = x_axis_.transform(x[i]+width/2.0) ;

        double base = ( bottom.empty() ) ? 0.0 : bottom[i] ;
        double y0 = y_axis_.transform(base) ;
        double y1 = y_axis_.transform(base + height[i]) ;

        c.save() ;
        c.setPen(pen) ;
        c.fillRect(x0, y0, x1-x0, y1-y0, brush) ;
        c.drawRect(x0, y0, x1-x0, y1-y0) ;
        c.restore() ;
    }
}

QRectF QBarChart::getDataBounds()
{


    qreal minx = numeric_limits<qreal>::max(),
            miny = numeric_limits<qreal>::max() ,
            maxx = -numeric_limits<qreal>::max(),
            maxy = -numeric_limits<qreal>::max() ;

    for( const xviz::BarSeries &ls: chart()->series() ) {
        const auto &x = ls.x() ;
        const auto &height = ls.height() ;
        const auto &bottom = ls.bottom() ;
        double w = ls.width() ;

        for( size_t i=0 ; i<x.size() ; i++ ) {
            double y0 = ( bottom.empty() ) ? 0.0 : bottom[i] ;
            double y1 = y0 + height[i] ;
            minx = std::min(minx, x[i]-w) ;
            miny = std::min(miny, y0) ;
            maxx = std::max(maxx, x[i]+w) ;
            maxy = std::max(maxy, y1) ;
        }
    }

    return QRectF(minx, miny, maxx - minx, maxy - miny) ;
}

QBarChart::QBarChart(const xviz::BarChart *lc): Chart(lc) {
}

void QBarChart::makeLegendEntries() {


    for( const xviz::BarSeries &ls: chart()->series()) {
        LegendEntry e ;
        e.flags_ = 0 ;
        e.label_ = QString::fromStdString(ls.title()) ;

        e.pen_ = qPenFromSolidPen(ls.pen()) ;
        e.brush_ = qBrushFromSolidBrush(ls.brush()) ;

        entries_.append(e) ;
    }
}
