#include <xviz/scene/camera.hpp>

#include <iostream>
#include <fstream>

#include <Eigen/Geometry>

using namespace std ;
using namespace Eigen ;

namespace xviz {

// http://antongerdelan.net/opengl/raycasting.html

Ray PerspectiveCamera::getRay(float sx, float sy) const
{
    float x = (2.0f * sx) / vp_.width_ - 1.0f;
    float y = 1.0f - (2.0f * sy) / vp_.height_;

    Vector4f ray_clip(x, y, -1, 1) ;

    Vector4f ray_eye = getProjectionMatrix().inverse() * ray_clip;
    ray_eye.z() = -1 ; ray_eye.w() = 0 ;

    Matrix4f mvi = mat_.inverse() ;
    Vector3f origin = mvi.block<3, 1>(0, 3);

    Vector3f ray_world = (mvi * ray_eye).head<3>().normalized() ;

    return Ray(origin, ray_world) ;
}

Eigen::Matrix4f PerspectiveCamera::projectionMatrix() const {
    assert(abs(aspect_ - std::numeric_limits<float>::epsilon()) > static_cast<float>(0));

//    float xfov = aspect_ * yfov_ ;
    float const d = 1/tan(yfov_ / static_cast<float>(2));

    Matrix4f result ;
    result.setZero() ;

    result(0, 0) = d / aspect_ ;
    result(1, 1) = d ;
    result(2, 2) =  (zfar_ + znear_) / (znear_ - zfar_);
    result(2, 3) =  2 * zfar_ * znear_ /(znear_ - zfar_) ;
    result(3, 2) = -1 ;

    return result;
}

Vector3f PerspectiveCamera::project(const Vector3f &p)
{
    Vector4f tmp { p.x(), p.y(), p.z(), 1.f };

    tmp = mat_ * tmp;
    tmp = getProjectionMatrix() * tmp;

    tmp /= tmp.w() ;

    tmp = tmp * 0.5 + Vector4f { 0.5, 0.5, 0.5, 0.5 } ;

    tmp[0] = tmp[0] * vp_.width_ + vp_.x_;
    tmp[1] = vp_.height_ - (tmp[1] * vp_.height_ + vp_.y_);

    return Vector3f { tmp[0], tmp[1], tmp[2] } ;
}


void Camera::lookAt(const Vector3f &eye, const Vector3f &center, const Vector3f &up) {
    Vector3f f = (center - eye).normalized();
    Vector3f s = f.cross(up).normalized();
    Vector3f u = s.cross(f) ;

    mat_ << s.x(), s.y(), s.z(), -s.dot(eye),
            u.x(), u.y(), u.z(), -u.dot(eye),
            -f.x(), -f.y(), -f.z(), f.dot(eye),
            0, 0, 0, 1 ;
}

void PerspectiveCamera::viewSphere(const Vector3f &center, float radius)
{

    // Ensure the sphere fits in the view port even if aspect ratio < 1 (i.e. width < height)
    float height = (1.05f * radius) / (aspect_ < 1.0f ? aspect_ : 1.0f);
    float dist = height / std::sin(yfov_/2.0) / 2.0f;

    Vector3f new_pos = center - Vector3f{0, 0, dist};
    lookAt(new_pos, center, Vector3f{0, 1, 0}) ;

}

void Camera::lookAt(const Vector3f &eye, const Vector3f &center, float roll) {
    lookAt(eye, center, Vector3f(0, 1, 0)) ;

    Affine3f rot ;
    rot.setIdentity();
    rot.rotate(AngleAxisf(roll, Eigen::Vector3f::UnitZ())) ;
    mat_ = mat_ * rot.matrix() ;
}

Ray::Ray(const Vector3f &orig, const Vector3f &dir) : orig_(orig), dir_(dir) {
    invdir_ = Vector3f(1.0f/dir.x(), 1.0f/dir.y(), 1.0f/dir.z());
    sign_[0] = (invdir_.x() < 0);
    sign_[1] = (invdir_.y() < 0);
    sign_[2] = (invdir_.z() < 0);
}

Ray::Ray(const Ray &other, const Eigen::Affine3f &tr):
    Ray((tr * other.orig_.homogeneous()).head<3>(), tr.linear() * other.dir_) {
    dir_.normalize() ;
}

Ray OrthographicCamera::getRay(float x, float y) const
{

}

Matrix4f OrthographicCamera::getProjectionMatrix() const {
    Matrix4f res ;

    res <<  2.0f/(right_ - left_), 0, 0, 0,
            0, 2.0f/(top_ - bottom_), 0, 0,
            0, 0, -2.0f/(far_ - near_), 0,
    - (right_ + left_) / (right_ - left_), - (top_ + bottom_) / (top_ - bottom_), - (far_ + near_) / (far_ - near_), 1.0;

    return res ;
}

Vector3f Camera::eye() const {
    auto v = getViewMatrix().inverse() * Vector4f(0, 0, -1, 1)  ;
    return v.block<3, 1>(0, 0) ;
}

} // namespace viz

