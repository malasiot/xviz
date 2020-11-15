#ifndef XVIZ_LINE_CHART_HPP
#define XVIZ_LINE_CHART_HPP

#include <xviz/chart.hpp>

namespace xviz {

namespace msg {
    class Chart ;
    class LineChart ;
}
class LineSeries: public ChartSeries {
public:
    LineSeries() = default ;

    LineSeries(const std::vector<double> &x, const std::vector<double> &y, const char *ps = nullptr) ;

    LineSeries &setErrors(const std::vector<double> &e) { e_ = e ; return *this; }

    LineSeries &setPen(const Pen &pen) { pen_ = pen; return *this; }
    Pen pen() const { return pen_ ; }

    LineSeries &setBrush(const Brush &brush) { brush_ = brush ; return *this ; }
    Brush brush() const { return brush_ ; }

    LineSeries & setMarker(const Marker &marker) { marker_ = marker ; return *this ; }
    Marker marker() const { return marker_ ; }

    void parseParamString(const char *ps) ;

    std::vector<double> &x() { return x_ ; }
    std::vector<double> &y() { return y_ ; }
    std::vector<double> &e() { return e_ ; }

    const std::vector<double> &x() const { return x_ ; }
    const std::vector<double> &y() const { return y_ ; }
    const std::vector<double> &e() const { return e_ ; }

private:

    Pen pen_  ;
    Brush brush_ ;
    Marker marker_ ;

    std::vector<double> x_, y_, e_ ;
};


class LineChart: public Chart {
public:
    void addSeries(LineSeries &s) { lines_.emplace_back(s) ; }
    const std::vector<LineSeries> &series() const { return lines_ ; }

    static void write(msg::Chart &chart, const LineChart *c)  ;
    static Chart *read(const msg::Chart &c)  ;

private:

    std::vector<LineSeries> lines_ ;

};

} // namespace xviz

#endif
