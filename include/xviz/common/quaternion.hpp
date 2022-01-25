#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include <xviz/common/vector3.hpp>
#include <xviz/common/matrix3x3.hpp>
#include <ostream>

namespace xviz {

class Quaternion {
public:
    Quaternion(): s_(1), v_(0, 0, 0) {}
    Quaternion(const Quaternion &q): s_(q.s_), v_(q.v_) {}
    Quaternion(float s, const Vector3 &v): s_(s), v_(v) {}
    Quaternion(float s, float x, float y, float z): s_(s), v_(x, y, z) {}
    Quaternion(const Matrix3x3 &m);
    Quaternion(const Vector3 &axis, float angle);

    Matrix3x3 toMatrix() const ;
    static Quaternion fromAxisAngle(const Vector3 &axis, float angle) ;

    float & s()       { return s_; }
    Vector3  &v()     { return v_; }
    float   s() const { return s_; }
    const Vector3 & v() const { return v_; }

    Quaternion & operator += (const Quaternion &q) { s_ += q.s_;  v_ += q.v_;  return *this; }
    Quaternion & operator -= (const Quaternion &q) { s_ -= q.s_;  v_ -= q.v_;  return *this; }
    Quaternion & operator =  (const Quaternion &q);

    Quaternion inverse() const ;
    Quaternion conjugate() const ;
    Vector3 transform(const Vector3 &v) ;

    double length() {return sqrt((v_[0]*v_[0])+(v_[1]*v_[1])+(v_[2]*v_[2])+(s_*s_)); }

    friend std::ostream &operator << (std::ostream &strm, const Quaternion &q) ;

protected:
    float s_;
    Vector3 v_;
};

Quaternion operator * (const Quaternion &q1, const Quaternion &q2);
Quaternion operator * (const Quaternion &q, const Vector3 &v);
Quaternion operator * (const Vector3 &v, const Quaternion &q) ;
Quaternion operator * (float s, const Quaternion &q);
Quaternion operator + (const Quaternion &q1, const Quaternion &q2) ;
Quaternion operator - (const Quaternion &q1, const Quaternion &q2) ;

inline Quaternion::Quaternion (const Matrix3x3 &m) {
   float tr = m(0,0) + m(1,1) + m(2,2), s;
   if ( tr >= 0 ) {
      s = sqrt(tr + 1);
      s_ = 0.5 * s;
      s = 0.5 / s;
      v_[0] = m(2,1) - m(1,2) * s;
      v_[1] = m(0,2) - m(2,0) * s;
      v_[2] = m(1,0) - m(0,1) * s;
   } else {
      int i = 0;
      if (m(1,1) > m(0,0)) i = 1;
      if (m(2,2) > m(i,i)) i = 2;
      switch (i) {
       case 0:
         s = sqrt((m(0,0) - (m(1,1) + m(2,2))) + 1);
         v_[0] = 0.5 * s;
         s = 0.5 / s;
         v_[1] = (m(0,1) + m(1,0)) * s;
         v_[2] = (m(2,0) + m(0,2)) * s;
         s_    = (m(2,1) - m(1,2)) * s;
         break;
       case 1:
         s = sqrt((m(1,1) - (m(2,2) + m(0,0))) + 1);
         v_[1] = 0.5 * s;
         s = 0.5 / s;
         v_[2] = (m(1,2) + m(2,1)) * s;
         v_[0] = (m(0,1) + m(1,0)) * s;
         s_    = (m(0,2) - m(2,0)) * s;
         break;
       case 2:
         s = sqrt((m(2,2) - (m(0,0) + m(1,1))) + 1);
         v_[2] = 0.5 * s;
         s = 0.5 / s;
         v_[0] = (m(2,0) + m(0,2)) * s;
         v_[1] = (m(1,2) + m(2,1)) * s;
         s_    = (m(1,0) - m(0,1)) * s;
         break;
      }
   }
}

inline Matrix3x3 Quaternion::toMatrix() const {
   const float s = s_, x = v_.x(), y =v_.y(), z = v_.z(),
          sx2 = s*x*2, sy2 = s*y*2, sz2 = s*z*2,
          xx2 = x*x*2, xy2 = x*y*2, xz2 = x*z*2,
          yy2 = y*y*2, yz2 = y*z*2, zz2 = z*z*2;
   return {
       1-yy2-zz2,  xy2-sz2,  xz2+sy2,
       xy2+sz2,  1-xx2-zz2,  yz2-sx2,
       xz2-sy2,  yz2+sx2,  1-xx2-yy2
   };
}


inline Quaternion::Quaternion(const Vector3 &axis, float angle):
   s_(cos(0.5*angle)), v_(sin(0.5*angle)*axis) {}

inline Quaternion &
Quaternion::operator = (const Quaternion &q) {
    v_ = q.v(); s_ = q.s();
  return *this;
}

inline Quaternion
operator * (const Quaternion &q1, const Quaternion &q2) {
   return Quaternion(q1.s() * q2.s() - dot(q1.v(), q2.v()),
               q1.s() * q2.v() + q2.s() * q1.v() + cross(q1.v(), q2.v()));
}

inline Quaternion operator * (const Quaternion &q, const Vector3 &v) {
    return { -dot(q.v(), v), q.s() * v + cross(q.v(), v) };
}

inline Quaternion operator * (const Vector3 &v, const Quaternion &q) {
   return { -dot(v, q.v()), q.s() * v + cross(v, q.v()) };
}

inline Quaternion operator * (float s, const Quaternion &q) {
   return { s*q.s(), s*q.v() };
}


inline Quaternion operator + (const Quaternion &q1, const Quaternion &q2) {
   return { q1.s()+q2.s(), q1.v()+q2.v() };
}

inline Quaternion operator - (const Quaternion &q1, const Quaternion &q2) {
   return { q1.s()-q2.s(), q1.v()-q2.v() };
}

inline Quaternion Quaternion::inverse () const {
   float mag2 = s_ * s_ + dot(v_, v_);
   return { s_/mag2, -v_/mag2 };
}

inline Quaternion Quaternion::conjugate() const {
   return { s_, -v_ } ;
}


// for unit quaternions only
inline Vector3 Quaternion::transform (const Vector3 &v) {
//   return (q * v * conjugate(q)).V();
    return { (s_ * s_ - dot(v_, v_)) * v +
                 2.0 * dot(v, v_) * v_ +
                 2.0 * s_ * cross(v_, v) };
}



}


#endif
