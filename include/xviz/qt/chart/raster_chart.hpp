#ifndef XVIZ_QT_RASTER_CHART_HPP
#define XVIZ_QT_RASTER_CHART_HPP

#include "chart_widget.hpp"
#include "axis.hpp"
#include "legend.hpp"

#include <xviz/raster_chart.hpp>

class QRasterChart: public Chart {
public:
    QRasterChart(const xviz::RasterChart *rc);

    void paintChart(QPainter &p, const QSize &sz) override ;
    QRectF getDataBounds() override ;

    void makeLegendEntries() override ;

    const xviz::RasterChart *chart() const {
        return static_cast<const xviz::RasterChart *>(chart_.get()) ;
    }

    ColorRamp *colorRamp() const override { return cramp_.get() ; }

private:

    double c_min_, c_max_ ;

    std::unique_ptr<ColorRamp> cramp_ ;

    QImage render(const QSize &sz) ;
    QVector<qreal> getXCoords() ;
    QVector<qreal> getYCoords() ;

    QColor mapColor(double v);
    void bilinear(QImage &im, const QRectF &rect, const QColor &clr00, const QColor &clr01, const QColor &clr10, const QColor &clr11);
    void flat(QImage &im, const QRectF &rect, const QColor &clr);
};

#endif
