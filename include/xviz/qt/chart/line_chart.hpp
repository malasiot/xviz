#ifndef XVIZ_QT_LINE_CHART_HPP
#define XVIZ_QT_LINE_CHART_HPP

#include "chart_widget.hpp"
#include "axis.hpp"
#include "legend.hpp"

#include <xviz/line_chart.hpp>

class QLineChart: public Chart {
public:

    QLineChart(const xviz::LineChart *lc);

    void paintChart(QPainter &p, const QSize &) override ;
    QRectF getDataBounds() override ;

    const xviz::LineChart *chart() const {
        return static_cast<const xviz::LineChart *>(chart_.get()) ;
    }

    XAxis &xAxis() { return x_axis_ ; }
    YAxis &yAxis() { return y_axis_ ; }
    Legend &legend() { return legend_ ; }

private:

    void makeLegendEntries() override ;

    void paintLine(QPainter &p, const QPen &pen, const xviz::LineSeries &ls);
    void paintMarkers(QPainter &p, const QPixmap &px, const xviz::LineSeries &ls);
    void paintErrorBars(QPainter &c, const xviz::LineSeries &ls) ;
    void paintAnnotations(QPainter &c);

    XAxis x_axis_ ;
    YAxis y_axis_ ;
    Legend legend_ ;

    QVector<LegendEntry> entries_ ;

    qreal max_title_width_ = 200 ;
    qreal title_offset_ = 4 ;
    QFont title_font_ = QFont("Arial", 14);

    QBrush bg_brush_ = QBrush(Qt::white) ;
    QPen bg_pen_ ;

    QPen error_bar_pen_;
    QPen error_bar_cap_pen_;
    uint draw_error_bars_every_nth_ = 1 ;
    qreal error_bar_cap_width_ = 3.0 ;

    QRectF data_bounds_ ;
    std::unique_ptr<const xviz::LineChart> chart_ ;

};












#endif
