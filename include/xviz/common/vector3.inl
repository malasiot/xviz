
inline Vector3::Vector3(float v): x_(v), y_(v), z_(v) {
}

inline Vector3::Vector3(float x, float y, float z): x_(x), y_(y), z_(z) {
}

inline Vector3::Vector3(const Vector3 &o): x_(o.x_), y_(o.y_), z_(o.z_) {
}

inline Vector3 &Vector3::operator = (const Vector3 &o) {
    x_ = o.x_ ; y_ = o.y_ ; z_ = o.z_ ;
    return *this ;
}

inline float Vector3::x() const {
    return x_ ;
}

inline float &Vector3::x() {
    return x_ ;
}

inline float Vector3::y() const {
    return y_ ;
}

inline float &Vector3::y() {
    return y_ ;
}

inline float Vector3::z() const {
    return z_ ;
}

inline float &Vector3::z() {
    return z_ ;
}

inline Vector3 operator+(const Vector3 &v1, const Vector3 &v2) {
  return Vector3(v1.x_ + v2.x_, v1.y_ + v2.y_, v1.z_ + v2.z_) ;
}

inline Vector3 operator-(const Vector3 &v1, const Vector3 &v2) {
  return Vector3(v1.x_ - v2.x_, v1.y_ - v2.y_, v1.z_ - v2.z_) ;
}

inline const Vector3 &Vector3::operator +=(const Vector3 &v) {
  x_ += v.x_ ; y_ += v.y_ ; z_ += v.z_ ;
  return *this ;
}

inline const Vector3 &Vector3::operator -=(const Vector3 &v) {
  x_ -= v.x_ ; y_ -= v.y_ ; z_ -= v.z_ ;
  return *this ;
}

inline float dot(const Vector3 &v1, const Vector3 &v2) {
   return v1.x_ * v2.x_ + v1.y_ * v2.y_ + v1.z_ * v2.z_ ;
}

inline float Vector3::dot(const Vector3 &v2) const {
   return x_ * v2.x_ + y_ * v2.y_ + z_ * v2.z_ ;
}

inline Vector3 operator *(const Vector3 &v, float f) {
  return Vector3(v.x_*f, v.y_*f, v.z_*f) ;
}

inline Vector3 operator /(const Vector3 &v, float f) {
  return Vector3(v.x_/f, v.y_/f, v.z_/f) ;
}

inline Vector3 &Vector3::operator *=(float f) {
  x_ *= f ; y_ *= f ; z_ *= f ;
  return *this ;
}

inline Vector3 &Vector3::operator /=(float f) {
  x_ /= f ; y_ /= f ; z_ /= f ;
  return *this ;
}

inline std::ostream &operator << (std::ostream &strm, const Vector3 &m) {
  strm << m.x_ << ' ' << m.y_ << ' ' << m.z_ ; return strm ;
}

inline float& Vector3::operator[] (size_t i) {
    assert((0<=i) && (i<=3));
    switch ( i ) {
    case 0: return x_ ;
    case 1: return y_ ;
    case 2: return z_ ;
    }
}
inline const float& Vector3::operator[] (size_t i) const {
    assert((0<=i) && (i<=3));
    switch ( i ) {
    case 0: return x_ ;
    case 1: return y_ ;
    case 2: return z_ ;
    }

}

inline Vector3 operator -(const Vector3 &v) { return Vector3(-v.x_, -v.y_, -v.z_) ; }

inline float Vector3::length() const { return sqrt(dot(*this)) ; }

inline float Vector3::norm() const { return length() ; }

inline float Vector3::squaredNorm() const { return dot(*this) ; }

inline void Vector3::normalize() { *this /= length() ; }

inline Vector3 Vector3::normalized() const { return *this / length() ; }

inline Vector3 cross(const Vector3 &v1, const Vector3 &v2) {
    return {v1.y_ * v2.z_ - v1.z_ * v2.y_, v1.z_ * v2.x_ - v1.x_ * v2.z_, v1.x_ * v2.y_ - v1.y_ * v2.x_} ;
}

inline Vector3 Vector3::cross(const Vector3 &v) const {
    return {y_ * v.z_ - z_ * v.y_, z_ * v.x_ - x_ * v.z_, x_ * v.y_ - y_ * v.x_} ;
}
