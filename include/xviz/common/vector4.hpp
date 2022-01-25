#ifndef XVIZ_VECTOR4_HPP
#define XVIZ_VECTO4_HPP

#include <iostream>
#include <cassert>
#include <cmath>

#include <xviz/common/vector3.hpp>

namespace xviz {


class Vector4 {
public:

    Vector4() = default ;
    Vector4(float v) ;
    Vector4(float x, float y, float z, float w) ;
    Vector4(const Vector4 &v) ;
    Vector4(const Vector3 &v, float w) ;

    float x() const ;
    float &x() ;
    float y() const ;
    float &y() ;
    float z() const ;
    float &z() ;
    float w() const ;
    float &w() ;

    Vector4 &operator=(const Vector4 &v) ;

    friend Vector4 operator+(const Vector4 &v1, const Vector4 &v2) ;
    friend Vector4 operator-(const Vector4 &v1, const Vector4 &v2) ;

    const Vector4 &operator +=(const Vector4 &v) ;
    const Vector4 &operator -=(const Vector4 &v) ;

    Vector3 head() const ;

    float& operator[] (size_t i) ;
    const float& operator[] (size_t i) const ;

    friend float dot(const Vector4 &v1, const Vector4 &v2) ;
    float dot(const Vector4 &other) const ;

    friend Vector4 operator *(const Vector4 &v, float f) ;
    friend Vector4 operator /(const Vector4 &v, float f) ;
    Vector4 &operator *=(float f) ;
    Vector4 &operator /=(float f) ;
    friend Vector4 operator *(float f, const Vector4 &b) { return b*f ; }
    friend Vector4 operator *(const Vector4 &v, float f) ;
    friend Vector4 operator -(const Vector4 &v) ;

    float length() const ;
    float norm() const ;
    float squaredNorm() const ;

    void normalize() ;
    Vector4 normalized() const ;

    friend std::ostream &operator << (std::ostream &strm, const Vector4 &m) ;

    float x_, y_, z_, w_ ;
} ;

#include <xviz/common/vector4.inl>


}
#endif
