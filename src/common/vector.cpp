#ifndef XVIZ_VECTOR_HPP
#define XVIZ_VECTOR_HPP

#include <iostream>
#include <cassert>
#include <cmath>

namespace xviz {

class Vector3 {
public:

    Vector3() = default ;
    constexpr Vector3(float v) ;
    constexpr Vector3(float x, float y, float z) ;
    constexpr Vector3(const Vector3 &v) ;

    constexpr float x() const ;
    constexpr float &x() ;
    constexpr float y() const ;
    constexpr float &y() ;
    constexpr float z() const ;
    constexpr float &z() ;

    constexpr Vector3 &operator=(const Vector3 &v) ;

    constexpr friend Vector3 operator+(const Vector3 &v1, const Vector3 &v2) ;
    constexpr friend Vector3 operator-(const Vector3 &v1, const Vector3 &v2) ;

    constexpr const Vector3 &operator +=(const Vector3 &v) ;
    constexpr const Vector3 &operator -=(const Vector3 &v) ;

    constexpr float& operator[] (size_t i) ;
    constexpr const float& operator[] (size_t i) const ;

    constexpr friend float dot(const Vector3 &v1, const Vector3 &v2) ;
    constexpr float dot(const Vector3 &other) const ;

    constexpr friend Vector3 operator *(const Vector3 &v, float f) ;
    constexpr friend Vector3 operator /(const Vector3 &v, float f) ;
    constexpr Vector3 &operator *=(float f) ;
    constexpr Vector3 &operator /=(float f) ;
    constexpr friend Vector3 operator *(float f, const Vector3 &b) { return b*f ; }
    constexpr friend Vector3 operator *(const Vector3 &v, float f) ;
    constexpr friend Vector3 operator -(const Vector3 &v) ;

    constexpr float length() const ;
    constexpr float norm() const ;
    constexpr float squaredNorm() const ;

    constexpr void normalize() ;
    constexpr Vector3 normalized() const ;

    friend std::ostream &operator << (std::ostream &strm, const Vector3 &m) ;

    float x_, y_, z_ ;
} ;

constexpr Vector3::Vector3(float v): x_(v), y_(v), z_(v) {
}

constexpr Vector3::Vector3(float x, float y, float z): x_(x), y_(y), z_(z) {
}

constexpr Vector3::Vector3(const Vector3 &o): x_(o.x_), y_(o.y_), z_(o.z_) {
}

constexpr Vector3 &Vector3::operator = (const Vector3 &o) {
    x_ = o.x_ ; y_ = o.y_ ; z_ = o.z_ ;
    return *this ;
}

constexpr float Vector3::x() const {
    return x_ ;
}

constexpr float &Vector3::x() {
    return x_ ;
}

constexpr float Vector3::y() const {
    return y_ ;
}

constexpr float &Vector3::y() {
    return y_ ;
}

constexpr float Vector3::z() const {
    return z_ ;
}

constexpr float &Vector3::z() {
    return z_ ;
}

constexpr Vector3 operator+(const Vector3 &v1, const Vector3 &v2) {
  return Vector3(v1.x_ + v2.x_, v1.y_ + v2.y_, v1.z_ + v2.z_) ;
}

constexpr Vector3 operator-(const Vector3 &v1, const Vector3 &v2) {
  return Vector3(v1.x_ - v2.x_, v1.y_ - v2.y_, v1.z_ - v2.z_) ;
}

constexpr const Vector3 &Vector3::operator +=(const Vector3 &v) {
  x_ += v.x_ ; y_ += v.y_ ; z_ += v.z_ ;
  return *this ;
}

constexpr const Vector3 &Vector3::operator -=(const Vector3 &v) {
  x_ -= v.x_ ; y_ -= v.y_ ; z_ -= v.z_ ;
  return *this ;
}

constexpr float dot(const Vector3 &v1, const Vector3 &v2) {
   return v1.x_ * v2.x_ + v1.y_ * v2.y_ + v1.z_ * v2.z_ ;
}

constexpr float Vector3::dot(const Vector3 &v2) const {
   return x_ * v2.x_ + y_ * v2.y_ + z_ * v2.z_ ;
}

constexpr Vector3 operator *(const Vector3 &v, float f) {
  return Vector3(v.x_*f, v.y_*f, v.z_*f) ;
}

constexpr Vector3 operator /(const Vector3 &v, float f) {
  return Vector3(v.x_/f, v.y_/f, v.z_/f) ;
}

constexpr Vector3 &Vector3::operator *=(float f) {
  x_ *= f ; y_ *= f ; z_ *= f ;
  return *this ;
}

constexpr Vector3 &Vector3::operator /=(float f) {
  x_ /= f ; y_ /= f ; z_ /= f ;
  return *this ;
}

inline std::ostream &operator << (std::ostream &strm, const Vector3 &m) {
  strm << m.x_ << ' ' << m.y_ << ' ' << m.z_ ; return strm ;
}

constexpr float& Vector3::operator[] (size_t i) {
    assert((0<=i) && (i<=3));
    switch ( i ) {
    case 0: return x_ ;
    case 1: return y_ ;
    case 2: return z_ ;
    }
}
constexpr const float& Vector3::operator[] (size_t i) const {
    assert((0<=i) && (i<=3));
    switch ( i ) {
    case 0: return x_ ;
    case 1: return y_ ;
    case 2: return z_ ;
    }

}

constexpr Vector3 operator -(const Vector3 &v) { return Vector3(-v.x_, -v.y_, -v.z_) ; }

constexpr float Vector3::length() const { return sqrt(dot(*this)) ; }

constexpr float Vector3::norm() const { return length() ; }

constexpr float Vector3::squaredNorm() const { return dot(*this) ; }

constexpr void Vector3::normalize() { *this /= length() ; }

constexpr Vector3 Vector3::normalized() const { return *this / length() ; }


}
#endif
