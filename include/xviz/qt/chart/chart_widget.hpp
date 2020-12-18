#ifndef XVIZ_QT_CHART_WIDGET_HPP
#define XVIZ_QT_CHART_WIDGET_HPP

#include <QWidget>
#include <memory>

#include <xviz/annotation.hpp>
#include <xviz/chart.hpp>

#include "axis.hpp"
#include "legend.hpp"
#include "color_ramp.hpp"

class Chart ;

class ChartWidget: public QWidget {
    Q_OBJECT

public:
    ChartWidget(QWidget *parent) ;

    void setChart(Chart *c);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::unique_ptr<Chart> chart_ ;
};

class Chart {
public:

    Chart(const xviz::Chart *c) ;

    XAxis &xAxis() { return x_axis_ ; }
    YAxis &yAxis() { return y_axis_ ; }
    Legend &legend() { return legend_ ; }

    virtual ~Chart() {}
    virtual void build() ;

    virtual void paint(QPainter &p, const QRect &rect) ;
    virtual QRectF getDataBounds() = 0 ;
    virtual void makeLegendEntries() = 0 ;
    virtual void paintChart(QPainter &p, const QSize &sz) = 0;
    virtual ColorRamp *colorRamp() const { return nullptr ;}

    void setTitle(const QString &title) { title_ = title ; }
    const QString &getTitle() const { return title_ ; }

    void setAnnotations(const std::vector<xviz::Annotation> &annotations) {
        annotations_ = annotations ;
    }

protected:
    void paintAnnotations(QPainter &p, const xviz::Matrix2d &m) ;

    XAxis x_axis_ ;
    YAxis y_axis_ ;
    Legend legend_ ;

    QVector<LegendEntry> entries_ ;

    qreal max_title_width_ = 200 ;
    qreal title_offset_ = 4 ;
    QFont title_font_ = QFont("Arial", 14);

    QBrush bg_brush_ = QBrush(Qt::white) ;
    QPen bg_pen_ ;

    QString title_ ;
    std::vector<xviz::Annotation> annotations_ ;
    QRectF data_bounds_ ;

    std::unique_ptr<const xviz::Chart> chart_ ;
};


#endif
