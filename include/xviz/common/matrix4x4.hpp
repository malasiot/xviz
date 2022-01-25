#ifndef XVIZ_MATRIX4x4_HPP
#define XVIZ_MATRIX4x4_HPP

#include <xviz/common/vector4.hpp>
#include <xviz/common/matrix3x3.hpp>

namespace xviz {

class Matrix4x4 {
public:

    Matrix4x4() = default ;
    Matrix4x4(float v): Matrix4x4(Vector4{v, v, v, v}) {}
    Matrix4x4(const Matrix4x4 &other) ;
    Matrix4x4(const Matrix3x3 &rs, const Vector3 &t) ;
    Matrix4x4(const Matrix3x3 &rs) ;

    // initialize from column vectors
    Matrix4x4(const Vector4 &v1, const Vector4 &v2, const Vector4 &v3, const Vector4 &v4) ;

    Matrix4x4(float a11, float a12, float a13, float a14,
              float a21, float a22, float a23, float a24,
              float a31, float a32, float a33, float a34,
              float a41, float a42, float a43, float a44) ;

    // Diagonal matrix

    Matrix4x4(const Vector4 &diag) ;

    void setUpperLeft(const Matrix3x3 &ul) ;

    Matrix3x3 upperLeft() const ;

    void setZero();
    void setIdentity();

    static Matrix4x4 identity() ;

    // Indexing operators

    float& operator() (int i, int j) ;
    float *operator[] (int i) ;
    const float *operator[] (int i) const ;

    Vector4 row(int r) const ;
    Vector4 column(int c) const ;

    void setRow(int r, const Vector4 &) ;
    void setColumn(int c, const Vector4 &) ;

    Matrix4x4 &operator = (const Matrix4x4 &) ;

    friend std::ostream &operator << (std::ostream &strm, const Matrix4x4 &m) ;

    friend Matrix4x4 operator + (const Matrix4x4 &m1, const Matrix4x4 &m2) ;
    friend Matrix4x4 operator - (const Matrix4x4 &m1, const Matrix4x4 &m2) ;
    friend Matrix4x4 operator * (const Matrix4x4 &m1, const Matrix4x4 &m2) ;
    friend Matrix4x4 operator * (const Matrix4x4 &m1, float s) ;
    friend Matrix4x4 operator / (const Matrix4x4 &m1, float s) { return m1 * (1/s) ; }
    friend Matrix4x4 operator * (float s, const Matrix4x4 &m1) { return m1*s ; }
    friend Vector4 operator * (const Matrix4x4 &m1, const Vector4 &v) ;
    friend Vector4 operator * (const Vector4 &v, const Matrix4x4 &m1) ;

    Matrix4x4 &operator += (const Matrix4x4 &m) ;
    Matrix4x4 &operator -= (const Matrix4x4 &m) ;

    Matrix4x4 &operator *= (const Matrix4x4 &m) ;
    Matrix4x4 &operator *= (float s) ;
    Matrix4x4 &operator /= (float s) ;

    friend Matrix4x4 operator - (const Matrix4x4 &);

    // Compute the inverse (inv) and determinant (det) of a matrix
    Matrix4x4 inverse(bool *invertible = nullptr) const ;
    void invert(bool *invertible) ;

    // Compute the transpose (float) of a matrix
    Matrix4x4 transpose() const ;
    void tranpose() ;

    // Return the determinant
    float det() const ;

    // Matrix elements

protected:

    float m_[4][4] ;
} ;

inline void Matrix4x4::setUpperLeft(const Matrix3x3 &ul) {
    m_[0][0] = ul(0, 0) ; m_[0][1] = ul(0, 1) ; m_[0][2] = ul(0, 2) ;
    m_[1][0] = ul(1, 0) ; m_[1][1] = ul(1, 1) ; m_[1][2] = ul(1, 2) ;
    m_[2][0] = ul(2, 0) ; m_[2][1] = ul(2, 1) ; m_[2][2] = ul(2, 2) ;
}

inline Matrix3x3 Matrix4x4::upperLeft() const {
    return {
        m_[0][0], m_[0][1], m_[0][2],
        m_[1][0], m_[1][1], m_[1][2],
        m_[2][0], m_[2][1], m_[2][2]
    };
}

#include <xviz/common/matrix4x4.inl>
}

#endif
