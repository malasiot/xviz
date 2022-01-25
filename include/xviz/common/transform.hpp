#ifndef XVIZ_TRANSFORM_HPP
#define XVIZ_TRANSFORM_HPP

#include <xviz/common/quaternion.hpp>
#include <xviz/common/matrix4x4.hpp>

namespace xviz {

class Transform: public Matrix4x4 {

public:
    Transform(): Matrix4x4(1.f) {}
    Transform(const Transform &o): Matrix4x4(o) {}
    Transform(const Matrix4x4 &t): Matrix4x4(t) {}

    Matrix3x3 linear() const { return upperLeft() ; }
    Vector3 translation() const { return column(3).head() ; }

    friend Vector3 operator * (const Transform &t, const Vector3 &v) {
        return (t * Vector4(v, 1.f)).head() ;
    }

    void setTranslation(const Vector3 &t) {
        m_[0][3] = t.x() ; m_[1][3] = t.y() ; m_[2][3] = t.y() ;
    }

    void setLinear(const Matrix3x3 &l) {
        setUpperLeft(l) ;
    }

    void translate(const Vector3 &tr) ;
    void rotate(const Matrix3x3 &rot) ;
    void rotate(const Quaternion &q) ;
    void scale(const Vector3 &s) ;
    void scale(float s) ;
};

inline void Transform::translate(const Vector3 &tr) {
    Vector3 v = linear() * tr ;
    m_[0][3] += v.x() ;  m_[1][3] += v.y() ; m_[2][3] += v.z() ;
}

inline void Transform::rotate(const Matrix3x3 &rot) {
    setUpperLeft(upperLeft() * rot) ;
}

inline void Transform::rotate(const Quaternion &q) {
    setUpperLeft(upperLeft() * q.toMatrix()) ;
}

inline void Transform::scale(const Vector3 &s) {
    setUpperLeft(upperLeft() * Matrix3x3(s)) ;
}

inline void Transform::scale(float s) {
    setUpperLeft(upperLeft() * Matrix3x3(s)) ;
}
}

#endif
