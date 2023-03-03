#pragma once

#include <xviz/overlay/overlay.hpp>

namespace xviz {

class Frame: public OverlayContainer {
public:
    Frame(OverlayContainer *child = nullptr) ;

    void setBorderWidth(float bw) { bw_ = bw ; }
    void setBorderColor(const Eigen::Vector3f &clr) { stroke_ = clr ; }
    void setBackgroundColor(const Eigen::Vector3f &fill) { fill_ = fill ;}
    void setOpacity(float a) { opacity_ = a; }

    void layout() override ;


protected:

    void measure(float &mw, float &mh) override ;

    void draw() ;

    float bw_ = 1;
    Eigen::Vector3f fill_ = {1, 1, 1}, stroke_ = {0, 0, 0} ;
    float opacity_ = 1 ;
    std::unique_ptr<OverlayContainer> child_ ;
};


}
