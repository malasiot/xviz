#ifndef XVIZ_SCENE_CAMERA_HPP
#define XVIZ_SCENE_CAMERA_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace xviz {

// Abstract camera

struct Viewport {
    Viewport() = default ;
    size_t x_ = 0, y_ = 0, width_, height_ ;
};

struct Ray
{
    public:
    Ray(const Eigen::Vector3f &orig, const Eigen::Vector3f &dir);

    Ray(const Ray &other, const Eigen::Affine3f &tr) ;

    Eigen::Vector3f origin() const { return orig_ ; }
    Eigen::Vector3f dir() const { return dir_ ; }

    Eigen::Vector3f orig_, dir_;
    Eigen::Vector3f invdir_ ;
    int sign_[3];
};

class Camera {
public:

    Camera():  mat_(Eigen::Matrix4f::Identity()) {}
    virtual ~Camera() {}

    void setViewport(size_t w, size_t h) {
        vp_.width_ = w ; vp_.height_ = h ;
    }

    void setViewport(size_t x, size_t y, size_t w, size_t h) {
        vp_.width_ = w ; vp_.height_ = h ;
        vp_.x_ = x ; vp_.y_ = y ;
    }

    void setViewTransform(const Eigen::Matrix4f &vt) {
        mat_ = vt ;
    }

    void setBgColor(const Eigen::Vector4f &clr) {
        bg_clr_ = clr ;
    }

    virtual Ray getRay(float x, float y) const = 0 ;

    virtual Eigen::Matrix4f getProjectionMatrix() const = 0;

    Eigen::Matrix4f getViewMatrix() const { return mat_ ; }
    const Viewport &getViewport() const { return vp_ ; }
    Eigen::Vector4f bgColor() const { return bg_clr_ ; }

    void lookAt(const Eigen::Vector3f &eye, const Eigen::Vector3f &center, float roll = 0);
    void lookAt(const Eigen::Vector3f &eye, const Eigen::Vector3f &center, const Eigen::Vector3f &up);
    Eigen::Vector3f eye() const;

    void setAspectRatio(float asp) {
        aspect_ = asp ;
    }

protected:

    Eigen::Matrix4f mat_ ; // view transformation
    Viewport vp_ ;
    Eigen::Vector4f bg_clr_ = {0, 0, 0, 1} ;
    float aspect_ = 1.0 ;
};

// Perspective camera

class PerspectiveCamera: public Camera {
public:
    PerspectiveCamera(float aspect, float yfov, float znear = 0.01, float zfar = 10.0):
        yfov_(yfov), znear_(znear), zfar_(zfar) {
    }

    Eigen::Matrix4f getProjectionMatrix() const override {
        return projectionMatrix() ;
    }

    Ray getRay(float x, float y) const override ;

    Eigen::Matrix4f projectionMatrix() const ;
    float zNear() const { return znear_ ; }
    float zFar() const { return zfar_ ; }

    float yFov() const { return yfov_ ; }

    void viewSphere(const Eigen::Vector3f &center, float radius) ;

    Eigen::Vector3f project(const Eigen::Vector3f &p) const ;
    Eigen::Vector3f unProject(float wx, float wy, float z) const ;
protected:

    float yfov_, znear_, zfar_ ;
};

// Orthographic camera

class OrthographicCamera: public Camera {
public:
    OrthographicCamera(float left, float right, float top, float bottom, float near = 0.01, float far = 10.0):
        left_(left), right_(right), top_(top), bottom_(bottom), near_(near), far_(far) {}

    Ray getRay(float x, float y) const override ;

    Eigen::Matrix4f getProjectionMatrix() const override ;

    void setAspect(float a) {
        aspect_ = a ;
    }

    float left() const { return left_ ; }
    float right() const { return right_ ; }
    float top() const { return top_ ; }
    float bottom() const { return bottom_ ; }
    float znear() const { return near_ ; }
    float zfar() const { return far_ ; }

protected:
    float left_, right_, top_, bottom_, near_, far_ ;


};


} // namespace clsim


#endif
