#ifndef XVIZ_QT_CHART_AXIS_HPP
#define XVIZ_QT_CHART_AXIS_HPP

#include <string>
#include <memory>

#include "tics.hpp"
#include <QPainter>


class Axis {
public:

    enum TicsPlacement { TicsInside, TicsOutside } ;

    Axis() {
        grid_pen_.setDashPattern({3, 10, 3, 10}) ;
    }

    Axis &setRange(qreal min_v, qreal max_v) {
        min_v_ = min_v ; max_v_ = max_v ;
        has_range_ = true ;
        return *this ;
    }

    Axis &setMargin(qreal margin) {
        margin_ = margin ;
        return *this ;
    }

    Axis &setTitle(const QString &title) { title_ = title ; return *this ; }
    Axis &setTickFormatter(TickFormatter tf) { tick_formatter_ = tf ; return *this ; }
    Axis &setLogarithmic(bool loga) { is_log_ = loga ; return *this ; }
    Axis &setGrid(bool grid) { draw_grid_ = grid ; return *this ; }
    Axis &setTickLocator(TickLocator *loc) { tick_locator_.reset(loc) ; return *this ;}

    Axis &setTickLocations(const std::vector<double> &tp) ;
    Axis &setTickLabels(const QVector<QString> &labels) ;

    double getScale() const { return scale_ ; }
    double getOffset() const { return offset_ ; }
    bool isLogarithmic() const { return is_log_ ; }

    qreal size() { return size_ ; }
    void computeTransform(qreal ws) ;

protected:
    friend class Chart ;
    static TickFormatter nullFormatter, defaultFormatter ;

    // compute tic positions
    void computeAxisLayout(qreal ls, qreal wsize);

    QRect paintLabel(QPainter &c, const QString &label, qreal x, qreal y, bool rotate) ;

protected:


    qreal margin_ = 0.0 ;
    qreal label_sep_ = 40 ;
    bool is_log_ = false ;
    bool is_reversed_ = false ;
    qreal tic_size_ = 5 ;
    qreal tic_minor_size_ = 3 ;
    qreal label_offset_ = 5 ;
    qreal title_offset_ = 5 ;
    qreal title_wrap_ = 100 ;
    bool draw_grid_ = false ;
    QString title_ ;
    TickFormatter tick_formatter_= defaultFormatter ;
    std::unique_ptr<TickLocator> tick_locator_ = std::unique_ptr<TickLocator>(new AutoTickLocator()) ;

    TicsPlacement tics_placement_ = TicsOutside ;

    QFont label_font_ = QFont("Arial", 14) ;
    QFont title_font_ = QFont("Arial", 16) ;
    QBrush text_brush_ = QBrush(Qt::black) ;
    QPen tics_pen_ = QPen(), grid_pen_ ;


    // layout data

    qreal size_, max_label_width_, max_label_height_ ;
    qreal step_, ls_, title_extra_ ;
    qreal vscale_, power_ ; // scaled applied to the displayed label values
    qreal min_v_, max_v_ ;
    qreal min_label_v_, max_label_v_ ; // minimum/maximum displayed label value
    qreal scale_, offset_  ;
    QVector<QString> tick_labels_ ;
    std::vector<double> tick_locations_ ;
    bool has_range_ = false, has_fixed_tics_ = false ;

};

class XAxis: public Axis {
public:

    void computeLayout(qreal wsize);

    void draw(QPainter &canvas, qreal wsize, qreal hsize);

    // from data coordinates to device coordinates
    qreal transform(double x) ;
};

class YAxis: public Axis {
public:

    void computeLayout(qreal wsize);
    void draw(QPainter &canvas, qreal wsize, qreal hsize);

    // from data coordinates to device coordinates
    qreal transform(double x) ;
};



#endif
