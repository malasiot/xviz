#ifndef XVIZ_VECTOR_HPP
#define XVIZ_VECTOR_HPP

#include <iostream>
#include <cassert>
#include <cmath>

namespace xviz {

class Vector3 {
public:

    Vector3() = default ;
    Vector3(float v) ;
    Vector3(float x, float y, float z) ;
    Vector3(const Vector3 &v) ;

    float x() const ;
    float &x() ;
    float y() const ;
    float &y() ;
    float z() const ;
    float &z() ;

    static Vector3 AxisX() {
        return { 1.f, 0.f, 0.f } ;
    }
    static Vector3 AxisY() {
        return { 0.f, 1.f, 0.f } ;
    }
    static Vector3 AxisZ() {
        return { 0.f, 0.f, 1.f } ;
    }

    Vector3 &operator=(const Vector3 &v) ;

    friend Vector3 operator+(const Vector3 &v1, const Vector3 &v2) ;
    friend Vector3 operator-(const Vector3 &v1, const Vector3 &v2) ;

    const Vector3 &operator +=(const Vector3 &v) ;
    const Vector3 &operator -=(const Vector3 &v) ;

    float& operator[] (size_t i) ;
    const float& operator[] (size_t i) const ;

    friend float dot(const Vector3 &v1, const Vector3 &v2) ;
    float dot(const Vector3 &other) const ;
    friend Vector3 cross(const Vector3 &v1, const Vector3 &v2) ;
    Vector3 cross(const Vector3 &v) const ;

    friend Vector3 operator *(const Vector3 &v, float f) ;
    friend Vector3 operator /(const Vector3 &v, float f) ;
    Vector3 &operator *=(float f) ;
    Vector3 &operator /=(float f) ;
    friend Vector3 operator *(float f, const Vector3 &b) { return b*f ; }
    friend Vector3 operator *(const Vector3 &v, float f) ;
    friend Vector3 operator -(const Vector3 &v) ;

    float length() const ;
    float norm() const ;
    float squaredNorm() const ;

    void normalize() ;
    Vector3 normalized() const ;

    friend std::ostream &operator << (std::ostream &strm, const Vector3 &m) ;

    float x_, y_, z_ ;
} ;

#include <xviz/common/vector3.inl>


}
#endif
