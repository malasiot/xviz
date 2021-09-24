#ifndef XVIZ_SCENE_INTERSECT_HPP
#define XVIZ_SCENE_INTERSECT_HPP

#include <Eigen/Core>
#include <xviz/scene/camera.hpp>

namespace xviz { namespace detail {

class AABB
{
    public:
    ~AABB()  ;
    AABB(const Eigen::Vector3f &b0, const Eigen::Vector3f &b1) {
        bounds_[0] = b0 ;
        bounds_[1] = b1;
    }

    Eigen::Vector3f bounds_[2];
};

bool rayIntersectsAABB(const Ray &, const AABB &box, float &t) ;
bool rayIntersectsTriangle(const Ray ray,
                           const Eigen::Vector3f &v0,
                           const Eigen::Vector3f &v1,
                           const Eigen::Vector3f &v2,
                           bool back_face_culling,
                           float &t) ;
bool rayIntersectsSphere(const Ray &ray, const Eigen::Vector3f &center, float radius, float &t) ;

bool triangleInsideBox(const Eigen::Vector3f &tv0, const Eigen::Vector3f &tv1,
                   const Eigen::Vector3f &tv2, const Eigen::Vector3f &boxcenter, const Eigen::Vector3f &boxhalfsize) ;

bool triangleIntersectsBox(const Eigen::Vector3f &tv0, const Eigen::Vector3f &tv1,
                   const Eigen::Vector3f &tv2, const Eigen::Vector3f &boxcenter, const Eigen::Vector3f &boxhalfsize) ;

bool triangleOutsideBox(const Eigen::Vector3f &v0, const Eigen::Vector3f &v1,
                   const Eigen::Vector3f &v2, const Eigen::Vector3f &boxcenter, const Eigen::Vector3f &boxhalfsize) ;

bool rayIntersectsPoint(const Ray &ray, const Eigen::Vector3f &p, float thresh, float &t) ;

bool rayIntersectsLine(const Ray &ray, const Eigen::Vector3f &p1, const Eigen::Vector3f &p2, float thresh, float &t) ;

bool rayIntersectsCylinder(const Ray &ray, float radius, float height, float &t) ;
}}
#endif
