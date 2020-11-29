#ifndef XVIZ_RASTER_CHART_HPP
#define XVIZ_RASTER_CHART_HPP

#include <xviz/chart.hpp>
#include <xviz/colormap.hpp>

namespace xviz {

namespace msg {
    class Chart ;
    class RasterChart ;
}

class RasterChart: public Chart {
public:

    enum Shading { Flat, Gouraud } ;

    RasterChart(uint w, uint h, const std::vector<double> &c,
                const std::vector<double> &x = {},
                const std::vector<double> &y = {}): width_(w), height_(h),
        c_(c), x_(x), y_(y) {}

    void setColormap(int cmap) {
        cmap_ = cmap ;
    }

    void setShading(Shading sh) { shading_ = sh ; }
    Shading shading() const { return shading_ ; }

    void setValueRange(double vmin, double vmax) {
        vrange_.reset(new std::pair<double, double>(vmin, vmax));
    }

    std::pair<double, double> *valueRange() const {
        return vrange_.get() ;
    }

    int colorMap() const { return cmap_ ; }

    const std::vector<double> &c() const { return c_ ; }
    const std::vector<double> &x() const { return x_ ; }
    const std::vector<double> &y() const { return y_ ; }

    uint width() const { return width_ ; }
    uint height() const { return height_ ; }

    static void write(msg::Chart &chart, const RasterChart *c)  ;
    static Chart *read(const msg::Chart &c)  ;

private:
    uint width_, height_ ;
    std::vector<double> x_, y_, c_ ;
    int cmap_ = ColorMapHot ;
    Shading shading_ = Gouraud ;
    std::unique_ptr<std::pair<double, double>> vrange_ ;
};

} // namespace xviz

#endif
