#include <xviz/xform.hpp>

#include "session.pb.h"

namespace xviz {

Matrix2d Matrix2d::read(const msg::Matrix2d &m) {
    return Matrix2d(m.m1(), m.m2(), m.m3(), m.m4(), m.m5(), m.m6()) ;
}

msg::Matrix2d *Matrix2d::write(const Matrix2d &m) {
    msg::Matrix2d *msg = new msg::Matrix2d ;
    msg->set_m1(m.m1_) ;
    msg->set_m2(m.m2_) ;
    msg->set_m3(m.m3_) ;
    msg->set_m4(m.m4_) ;
    msg->set_m5(m.m5_) ;
    msg->set_m6(m.m6_) ;
    return msg ;
}

}
