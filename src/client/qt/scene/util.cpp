#include "util.hpp"

QMatrix4x4 eigenToQt(const Eigen::Matrix4f &transform) {
    return QMatrix4x4(transform.data()).transposed();
}

QMatrix3x3 eigenToQt(const Eigen::Matrix3f &transform) {
    return QMatrix3x3(transform.data()).transposed();
}
