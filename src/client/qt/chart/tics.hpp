#ifndef XVIZ_QT_CHART_TICS_HPP
#define XVIZ_QT_CHART_TICS_HPP

#include <vector>
#include <functional>

#include <QString>

using TickFormatter = std::function<QString(double v, int idx)> ;

class TickLocator {
public:
    // override to define custom tick positions
    virtual void compute(double vmin, double vmax, uint maxTics, double minStep, double &label_min, double &label_max, std::vector<double> &loc) = 0;
};

class AutoTickLocator: public TickLocator {
public:
    void compute(double vmin, double vmax, uint maxTics, double minStep, double &label_min, double &label_max, std::vector<double> &loc) override;

protected:
    // round to one decimal point
    static double sround(double x);
    // compute normalized coordinates and associated tics
    static void bounds(double sx, double xu, double xl, unsigned &nTics, double &rxu, double &rxl);
};


class FixedTickLocator: public TickLocator {
public:
    FixedTickLocator(const std::vector<double> &loc): loc_(loc) {}

    void compute(double vmin, double vmax, uint maxTics, double minStep, double &label_min, double &label_max, std::vector<double> &loc) override;

private:
    const std::vector<double> loc_ ;
};

#endif
