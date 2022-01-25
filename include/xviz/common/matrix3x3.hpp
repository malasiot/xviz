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
    float operator() (int i, int j) const ;
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

    Matrix3x3 &operator += (const Matrix3x3 &m) ;
    Matrix3x3 &operator -= (const Matrix3x3 &m) ;

    Matrix3x3 &operator *= (const Matrix3x3 &m) ;
    Matrix3x3 &operator *= (float s) ;
    Matrix3x3 &operator /= (float s) ;

    friend Matrix3x3 operator - (const Matrix3x3 &);

    // Compute the inverse (inv) and determinant (det) of a matrix
    Matrix3x3 inverse(bool *invertible = nullptr) const ;
    void invert(bool *invertible) ;

    // Compute the transpose (float) of a matrix
    Matrix3x3 transpose() const ;
    void tranpose() ;

    // Return the determinant
    float det() const ;

    static Matrix3x3 rotationX(float a) ;
    static Matrix3x3 rotationY(float a) ;
    static Matrix3x3 rotationZ(float a) ;
    static Matrix3x3 rotationAxisAngle(const Vector3 &axis, float a) ;

protected:

    // Matrix elements

    float m_[3][3] ;
} ;

inline Matrix3x3 Matrix3x3::rotationX(float a) {
   const float c = cos(a), s = sin(a);
   return { 1.f, 0.f, 0.f,
            0.f,   c,  -s,
            0.f,   s,   c } ;
}

inline Matrix3x3 Matrix3x3::rotationY(float a) {
   const float c = cos(a), s = sin(a);
   return {   c,   0.f,    s,
            0.f,   1.f,  0.f,
             -s,   0.f,   c } ;
}

inline Matrix3x3 Matrix3x3::rotationZ(float a) {
   const float c = cos(a), s = sin(a);
   return {   c,   s,  0.f,
              s,   c,  0.f,
            0.f, 0.f,  1.f } ;
}

inline Matrix3x3 Matrix3x3::rotationAxisAngle(const Vector3 &v, float a) {
    float s = sin(a), c = cos(a), c_1 = 1-c,
         xx = v.x()*v.x()*c_1, xy = v.x()*v.y()*c_1, xz = v.x()*v.z()*c_1,
         yy = v.y()*v.y()*c_1, yz = v.y()*v.z()*c_1, zz = v.z()*v.z()*c_1,
         xs = v.x()*s, ys = v.y()*s, zs = v.z()*s;
    return { xx+c, xy-zs, xz+ys,
            xy+zs, yy+c,  yz-xs,
            xz-ys, yz+xs,  zz+c };
}


#include <xviz/common/matrix3x3.inl>
}

#endif
