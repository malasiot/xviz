#ifndef XVIZ_CHART_HPP
#define XVIZ_CHART_HPP

#include <string>
#include <vector>
#include <memory>

#include <xviz/message.hpp>
#include <xviz/pen.hpp>
#include <xviz/brush.hpp>
#include <xviz/marker.hpp>
#include <xviz/annotation.hpp>

namespace xviz {

class ChartSeries {
public:
    void setTitle(const std::string &title) { title_ = title ; }
    const std::string &title() const { return title_ ; }

protected:
    std::string title_ ;


};

struct Tick {
    std::string label_ ;
    double pos_ ;
};

class ChartCodec ;

class Chart: public Message {
public:
    virtual ~Chart() = default ;

    void setTitle(const std::string &title) { title_ = title ; }
    const std::string &title() const { return title_ ; }

    const std::string &labelX() const { return label_x_ ; }
    const std::string &labelY() const { return label_y_ ; }

    void setLabelX(const std::string &l) { label_x_ = l ; }
    void setLabelY(const std::string &l) { label_y_ = l ; }

    void setTicksX(const std::vector<double> &pos, const std::vector<std::string> &labels = {});
    void setTicksY(const std::vector<double> &pos, const std::vector<std::string> &labels = {});

    const std::vector<Tick> &getTicksX() const { return ticks_x_ ; }
    const std::vector<Tick> &getTicksY() const { return ticks_y_ ; }

    void addAnnotation(const Annotation &a) {
        annotations_.push_back(a) ;
    }

    const std::vector<Annotation> &annotations() const {
        return annotations_ ;
    }

    std::string encode() const override ;
    static Chart *decode(const std::string &bytes);

protected:

    friend class ChartCodec ;

    std::string title_, label_x_, label_y_ ;
    std::vector<Tick> ticks_x_, ticks_y_ ;
    std::vector<Annotation> annotations_ ;

private:

    void setTicks(std::vector<Tick> &ticks, const std::vector<double> &pos, const std::vector<std::string> &labels);
};



}

#endif
