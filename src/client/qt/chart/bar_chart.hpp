#ifndef XVIZ_QT_BAR_CHART_HPP
#define XVIZ_QT_BAR_CHART_HPP

#include "chart_widget.hpp"
#include "axis.hpp"
#include "legend.hpp"

#include <xviz/bar_chart.hpp>

class QBarChart: public Chart {
public:

    QBarChart(const xviz::BarChart *lc);

    void paint(QPainter &p, const QRect &rect) override ;
    QRectF getDataBounds() override ;

    XAxis &xAxis() { return x_axis_ ; }
    YAxis &yAxis() { return y_axis_ ; }
    Legend &legend() { return legend_ ; }


private:

    void paintBars(QPainter &p, const QPen &pen, const QBrush &brush, const xviz::BarSeries &ls);
    void paintAnnotations(QPainter &c);
    void paintLabels(QPainter &c, const xviz::Matrix2d &m, const xviz::LabelAnnotation &la) ;
    void paintShapes(QPainter &c, const xviz::Matrix2d &m, const xviz::ShapeAnnotation &la);
    void paintMarkers(QPainter &c, const xviz::Matrix2d &m, const xviz::MarkerAnnotation &ma);

    XAxis x_axis_ ;
    YAxis y_axis_ ;
    Legend legend_ ;

    QVector<LegendEntry> entries_ ;

    qreal max_title_width_ = 200 ;
    qreal title_offset_ = 4 ;
    QFont title_font_ = QFont("Arial", 14);

    QBrush bg_brush_ = QBrush(Qt::white) ;
    QPen bg_pen_ ;

    QRectF data_bounds_ ;
    std::unique_ptr<const xviz::BarChart> chart_ ;

};

#endif
