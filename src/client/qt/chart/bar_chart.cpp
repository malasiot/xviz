#include "bar_chart.hpp"
#include "../qt_graphics_helpers.hpp"
#include <limits>
#include <QPainterPath>

using namespace std ;

void QBarChart::paint(QPainter &c, const QRect &rect) {

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

        paintBars(c, e.pen_, e.brush_, ls) ;
    }

    paintAnnotations(c);

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

void QBarChart::paintAnnotations(QPainter &c) {
    c.save() ;
 //   c.translate(x_axis_.getOffset(), y_axis_.getOffset());
 //   c.scale(x_axis_.getScale(), -y_axis_.getScale()) ;

    for( const auto &a: annotations_ ) {
        paintDrawable(c, a,
                      x_axis_.getOffset(), y_axis_.getOffset(),
                      x_axis_.getScale(), -y_axis_.getScale());
    }

    c.restore() ;

}

QRectF QBarChart::getDataBounds()
{
    qreal minx = numeric_limits<qreal>::max(),
            miny = numeric_limits<qreal>::max() ,
            maxx = -numeric_limits<qreal>::max(),
            maxy = -numeric_limits<qreal>::max() ;

    for( const xviz::BarSeries &ls: chart_->series() ) {
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

QBarChart::QBarChart(const xviz::BarChart *lc) {
    chart_.reset(lc) ;
    data_bounds_ = getDataBounds() ;

    y_axis_.setMargin(0.0);
    x_axis_.setRange(data_bounds_.topLeft().x(), data_bounds_.bottomRight().x()) ;
    y_axis_.setRange(data_bounds_.topLeft().y(), data_bounds_.bottomRight().y()) ;

    x_axis_.setTitle(QString::fromStdString(lc->labelX())) ;
    y_axis_.setTitle(QString::fromStdString(lc->labelY())) ;

    if ( !lc->getTicksX().empty() ) {
        vector<double> ticks ;
        QVector<QString> tick_labels ;
        for( const xviz::Tick &tick: lc->getTicksX() ) {
            ticks.push_back(tick.pos_) ;
            if ( !tick.label_.empty() )
                tick_labels.append(QString::fromStdString(tick.label_)) ;
        }

        x_axis_.setTickLocations(ticks);
        if ( !tick_labels.isEmpty() )
            x_axis_.setTickLabels(tick_labels) ;
    }

    if ( !lc->getTicksY().empty() ) {
        vector<double> ticks ;
        QVector<QString> tick_labels ;
        for( const xviz::Tick &tick: lc->getTicksY() ) {
            ticks.push_back(tick.pos_) ;
            if ( !tick.label_.empty() )
                tick_labels.append(QString::fromStdString(tick.label_));
        }

        y_axis_.setTickLocations(ticks);
        if ( !tick_labels.isEmpty() )
            y_axis_.setTickLabels(tick_labels) ;
    }

    for( const xviz::BarSeries &ls: lc->series()) {
        LegendEntry e ;
        e.flags_ = 0 ;
        e.label_ = QString::fromStdString(ls.title()) ;

        e.pen_ = qPenFromSolidPen(ls.pen()) ;
        e.brush_ = qBrushFromSolidBrush(ls.brush()) ;

        entries_.append(e) ;
    }
}
