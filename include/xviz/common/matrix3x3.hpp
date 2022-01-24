#ifndef XVIZ_MATRIX3x3_HPP
#define XVIZ_MATRIX3x3_HPP

#include <xviz/common/vector3.hpp>

namespace xviz {

class Matrix3x3 {
public:

    Matrix3x3() = default ;
    Matrix3x3(const Matrix3x3 &other) ;

    // initialize from column vectors
    Matrix3x3(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3) ;

    Matrix3x3(float a11, float a12, float a13,
              float a21, float a22, float a23,
              float a31, float a32, float a33) ;

    Matrix3x3(float a[9]) ;

    // Diagonal matrix

    Matrix3x3(const Vector3 &diag) ;

    void setZero();
    void setIdentity();

    static Matrix3x3 identity() ;
    // equals: v1 * v2'
    static Matrix3x3 outer(const Vector3 &v1, const Vector3 &v2) ;

    // Indexing operators

    float& operator() (int i, int j) ;
    float *operator[] (int i) ;
    const float *operator[] (int i) const ;

    Vector3 row(int r) const ;
    Vector3 column(int c) const ;

    void setRow(int r, const Vector3 &) ;
    void setColumn(int c, const Vector3 &) ;

    Matrix3x3 &operator = (const Matrix3x3 &) ;

    friend std::ostream &operator << (std::ostream &strm, const Matrix3x3 &m) ;

    friend Matrix3x3 operator + (const Matrix3x3 &m1, const Matrix3x3 &m2) ;
    friend Matrix3x3 operator - (const Matrix3x3 &m1, const Matrix3x3 &m2) ;
    friend Matrix3x3 operator * (const Matrix3x3 &m1, const Matrix3x3 &m2) ;
    friend Matrix3x3 operator * (const Matrix3x3 &m1, float s) ;
    friend Matrix3x3 operator / (const Matrix3x3 &m1, float s) { return m1 * (1/s) ; }
    friend Matrix3x3 operator * (float s, const Matrix3x3 &m1) { return m1*s ; }
    friend Vector3 operator * (const Matrix3x3 &m1, const Vector3 &v) ;
    friend Vector3 operator * (const Vector3 &v, const Matrix3x3 &m1) ;

    const Matrix3x3 &operator += (const Matrix3x3 &m) ;
    const Matrix3x3 &operator -= (const Matrix3x3 &m) ;

    const Matrix3x3 &operator *= (const Matrix3x3 &m) ;
    const Matrix3x3 &operator *= (float s) ;
    const Matrix3x3 &operator /= (float s) ;

    friend Matrix3x3 operator - (const Matrix3x3 &);

    // Compute the inverse (inv) and determinant (det) of a matrix
    Matrix3x3 inverse(bool *invertible = nullptr) const ;
    void invert(bool *invertible) ;

    // Compute the transpose (float) of a matrix
    Matrix3x3 transpose() const ;
    void tranpose() ;

    // Return the determinant
    float det() const ;

    // Matrix elements

    float m_[3][3] ;
} ;

inline Matrix3x3::Matrix3x3(float m11, float m12, float m13,
                            float m21, float m22, float m23,
                            float m31, float m32, float m33 ) {
    m_[0][0] = m11 ; m_[0][1] = m12 ; m_[0][2] = m13 ;
    m_[1][0] = m21 ; m_[1][1] = m32 ; m_[1][2] = m33 ;
    m_[2][0] = m31 ; m_[2][1] = m32 ; m_[2][2] = m33 ;
}


inline float& Matrix3x3::operator() (int i, int j) {
                assert((0<=i) && (i<=2) && (0<=j) && (j<=2));
                return m_[i][j] ;
}


inline float *Matrix3x3::operator[] (int i) {
    assert((0<=i) && (i<=2));
    return m_[i] ;
}


inline const float *Matrix3x3::operator[] (int i) const {
    assert((0<=i) && (i<=2));
    return m_[i];
}

inline void Matrix3x3::setZero() {
    m_[0][0] = m_[0][1] = m_[0][2] =
    m_[1][0] = m_[1][1] = m_[1][2] =
    m_[2][0] = m_[2][1] = m_[2][2] = 0.f ;
}

inline void Matrix3x3::setIdentity()
{
    m_[0][0] = m_[1][1] = m_[2][2] = 0.f ;

    m_[0][1] = m_[0][2] =
    m_[1][0] = m_[1][2] =
    m_[2][0] = m_[2][1] = 0.f ;
}

inline Matrix3x3 Matrix3x3::identity() {
    return { 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f } ;
}


inline Matrix3x3::Matrix3x3(const Matrix3x3 &m) {
    for( size_t i = 0 ; i < 3 ; i++ )
        for( size_t j = 0 ; j < 3 ; j++ )
            m_[i][j] = m.m_[i][j] ;
}


inline Matrix3x3 &Matrix3x3::operator = (const Matrix3x3 &m) {
    for( size_t i = 0 ; i < 3 ; i++ )
        for( size_t j = 0 ; j < 3 ; j++ )
            m_[i][j] = m.m_[i][j] ;

  return *this ;
}


inline Matrix3x3::Matrix3x3(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3) {
  m_[0][0] = v1.x() ; m_[0][1] = v2.x() ; m_[0][2] = v3.x() ;
  m_[1][0] = v1.y() ; m_[1][1] = v2.y() ; m_[1][2] = v3.y() ;
  m_[2][0] = v1.z() ; m_[2][1] = v2.z() ; m_[2][2] = v3.z() ;
}


inline Matrix3x3 Matrix3x3::outer(const Vector3 &v1, const Vector3 &v2) {
    return {
        v1.x() * v2.x(), v1.x() * v2.y(), v1.x() * v2.z(),
        v1.y() * v2.x(), v1.y() * v2.y(), v1.y() * v2.z(),
        v1.z() * v2.x(), v1.z() * v2.y(), v1.z() * v2.z()
    };
}


inline std::ostream &operator << (std::ostream &strm, const Matrix3x3 &m) {
  strm << m.m_[0][0] << ' ' << m.m_[0][1] << ' ' << m.m_[0][2] << std::endl ;
  strm << m.m_[1][0] << ' ' << m.m_[1][1] << ' ' << m.m_[1][2] << std::endl ;
  strm << m.m_[2][0] << ' ' << m.m_[2][1] << ' ' << m.m_[2][2] << std::endl ;

  return strm ;
}


inline Matrix3 operator + (const Matrix3 &m1, const Matrix3 &m2)
{
  Matrix3 T ;

  T.m11 = m1.m11 + m2.m11 ;  T.m12 = m1.m12 + m2.m12 ;  T.m13 = m1.m13 + m2.m13 ;
  T.m21 = m1.m21 + m2.m21 ;  T.m22 = m1.m22 + m2.m22 ;  T.m23 = m1.m23 + m2.m23 ;
  T.m31 = m1.m31 + m2.m31 ;  T.m32 = m1.m32 + m2.m32 ;  T.m33 = m1.m33 + m2.m33 ;

  return T ;
}


inline Matrix3 operator - (const Matrix3 &m1, const Matrix3 &m2)
{
  Matrix3 T ;

  T.m11 = m1.m11 - m2.m11 ;  T.m12 = m1.m12 - m2.m12 ;  T.m13 = m1.m13 - m2.m13 ;
  T.m21 = m1.m21 - m2.m21 ;  T.m22 = m1.m22 - m2.m22 ;  T.m23 = m1.m23 - m2.m23 ;
  T.m31 = m1.m31 - m2.m31 ;  T.m32 = m1.m32 - m2.m32 ;  T.m33 = m1.m33 - m2.m33 ;

  return T ;
}


inline Matrix3 operator - (const Matrix3 &m)
{
  return Matrix3(-m.m11, -m.m12, -m.m13, -m.m21, -m.m22, -m.m23, -m.m31, -m.m32, -m.m33) ;
}


inline void Matrix3::Transpose(Matrix3 &r) const
{
  r.m11 = m11 ; r.m12 = m21 ; r.m13 = m31 ;
  r.m21 = m12 ; r.m22 = m22 ; r.m23 = m32 ;
  r.m31 = m13 ; r.m32 = m23 ; r.m33 = m33 ;
}


inline Matrix3 Matrix3::Tran() const
{
  return Matrix3(m11, m21, m31, m12, m22, m32, m13, m23, m33) ;
}


inline Vector3s Matrix3::GetRow(int r) const {
  return Vector3s((*this)[r][0], (*this)[r][1], (*this)[r][2]) ;
}


inline Vector3s Matrix3::GetColumn(int c) const {
  return Vector3s((*this)[0][c], (*this)[1][c], (*this)[2][c]) ;
}


inline void Matrix3::SetRow(int r, const Vector3s &v) {
  (*this)[r][0] = v.m1 ;
  (*this)[r][1] = v.m2 ;
  (*this)[r][2] = v.m3 ;
}


inline void Matrix3::SetColumn(int c, const Vector3s &v) {
  (*this)[0][c] = v.m1 ;
  (*this)[1][c] = v.m2 ;
  (*this)[2][c] = v.m3 ;
}


inline Matrix3::Matrix3(const Vector3s &diag)
{
  m11 = diag.m1 ; m22 = diag.m2 ; m33 = diag.m3 ;
  m12 = m21 = m13 = m31 = m23 = m32 = 0.0 ;
}


inline Matrix3 operator * (const Matrix3 &m1, float s)
{
    Matrix3 T(m1) ; T *= s ; return T ;
}


inline Vector3s operator * (const Matrix3 &m1, const Vector3s &v)
{
  float s1, s2, s3, t1, t2, t3 ;

  s1 = v.m1 ; s2 = v.m2 ; s3 = v.m3 ;

  t1 = m1.m11 * s1 + m1.m12 * s2 + m1.m13 * s3 ;
  t2 = m1.m21 * s1 + m1.m22 * s2 + m1.m23 * s3 ;
  t3 = m1.m31 * s1 + m1.m32 * s2 + m1.m33 * s3 ;

  return Vector3s(t1, t2, t3) ;
}


inline Vector3s operator * (const Vector3s &v, const Matrix3 &m1)
{
  float s1, s2, s3, t1, t2, t3 ;

  s1 = v.m1 ; s2 = v.m2 ; s3 = v.m3 ;

  t1 = s1 * m1.m11 + s2 * m1.m21 + s3 * m1.m31 ;
  t2 = s1 * m1.m12 + s2 * m1.m22 + s3 * m1.m32 ;
  t3 = s1 * m1.m13 + s2 * m1.m23 + s3 * m1.m33 ;

  return Vector3s(t1, t2, t3) ;
}


inline const Matrix3 &Matrix3::operator += (const Matrix3 &m2)
{
  m11 += m2.m11 ;  m12 += m2.m12 ;  m13 += m2.m13 ;
  m21 += m2.m21 ;  m22 += m2.m22 ;  m23 += m2.m23 ;
  m31 += m2.m31 ;  m32 += m2.m32 ;  m33 += m2.m33 ;

  return *this ;
}


inline const Matrix3 &Matrix3::operator -= (const Matrix3 &m2)
{
  m11 -= m2.m11 ;  m12 -= m2.m12 ;  m13 -= m2.m13 ;
  m21 -= m2.m21 ;  m22 -= m2.m22 ;  m23 -= m2.m23 ;
  m31 -= m2.m31 ;  m32 -= m2.m32 ;  m33 -= m2.m33 ;

  return *this ;
}


inline const Matrix3 &Matrix3::operator *= (float s)
{
  m11 *= s ; m21 *= s ; m31 *= s ;
  m12 *= s ; m22 *= s ; m32 *= s ;
  m13 *= s ; m23 *= s ; m33 *= s ;

  return *this ;
}

#include <xviz/common/matrix3x3.inl>
}

#endif
