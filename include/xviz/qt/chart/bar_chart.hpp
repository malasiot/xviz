#ifndef XVIZ_QT_BAR_CHART_HPP
#define XVIZ_QT_BAR_CHART_HPP

#include "chart_widget.hpp"
#include "axis.hpp"
#include "legend.hpp"

#include <xviz/bar_chart.hpp>

class QBarChart: public Chart {
public:
    QBarChart(const xviz::BarChart *lc);

    void paintChart(QPainter &p, const QSize &) override ;
    QRectF getDataBounds() override ;

    void makeLegendEntries() override ;

    const xviz::BarChart *chart() const {
        return static_cast<const xviz::BarChart *>(chart_.get()) ;
    }

private:

    void paintBars(QPainter &p, const QPen &pen, const QBrush &brush, const xviz::BarSeries &ls);

};

#endif
