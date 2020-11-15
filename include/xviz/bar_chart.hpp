#ifndef XVIZ_BAR_CHART_HPP
#define XVIZ_BAR_CHART_HPP

#include <xviz/chart.hpp>

namespace xviz {

namespace msg {
    class Chart ;
}

class BarSeries: public ChartSeries {
public:
    enum BarPlacement { Center, Edge } ;

    BarSeries() = default ;

    BarSeries(const std::vector<double> &x, const std::vector<double> &h) ;

    BarSeries &setBottom(const std::vector<double> &b) { y0_ = b ; return *this; }

    BarSeries &setWidth(double w) { w_ = w ; return *this; }

    BarSeries &setPen(const Pen &pen) { pen_ = pen; return *this; }
    Pen pen() const { return pen_ ; }

    BarSeries &setBrush(const Brush &brush) { brush_ = brush ; return *this ; }
    Brush brush() const { return brush_ ; }

    std::vector<double> &x() { return x_ ; }
    std::vector<double> &height() { return h_ ; }
    double &width() { return w_ ; }
    std::vector<double> &bottom() { return y0_ ; }

    const std::vector<double> &x() const { return x_ ; }
    const std::vector<double> &height() const { return h_ ; }
    double width() const { return w_ ; }
    const std::vector<double> &bottom() const { return y0_ ; }

private:

    Pen pen_  ;
    Brush brush_ ;

    std::vector<double> x_, h_, y0_ ;
    double w_ ;
};


class BarChart: public Chart {
public:
    void addSeries(BarSeries &s) { series_.emplace_back(s) ; }
    const std::vector<BarSeries> &series() const { return series_ ; }

    static void write(msg::Chart &chart, const BarChart *c)  ;
    static Chart *read(const msg::Chart &c)  ;

private:

    std::vector<BarSeries> series_ ;

};

} // namespace xviz

#endif
