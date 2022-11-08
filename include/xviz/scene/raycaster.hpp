#ifndef XVIZ_SCENE_RAYCASTER_HPP
#define XVIZ_SCENE_RAYCASTER_HPP

#include <xviz/scene/camera.hpp>
#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/drawable.hpp>

namespace xviz {

namespace detail {
    class Octree ;
    class AABB ;
}

struct RayCastResult {
    NodePtr node_ ;
    Drawable *drawable_ = nullptr ;

    float t_ ; // distance from ray origin to geometry hit

    uint32_t point_idx_ ;
    Eigen::Vector3f pt_ ; // intersection point
    uint32_t triangle_idx_[3] ; // index of triangle vertices
    uint32_t line_idx_[2] ; // index of triangle vertices
};

class RayCaster {
public:
    ~RayCaster() ;
    RayCaster() ;

    bool intersect(const Ray &ray, const NodePtr &scene, std::vector<RayCastResult> &results) ;
    bool intersect(const Ray &ray, const std::vector<NodePtr> &nodes, std::vector<RayCastResult> &results) ;

    bool intersectOne(const Ray &ray, const std::vector<NodePtr> &nodes, RayCastResult &results) ;
    bool intersectOne(const Ray &ray, const NodePtr &scene, RayCastResult &result);

    void setPointDistanceThreshold(float t) {
        point_distance_thresh_sq_ = t * t;
    }

    void setLineDistanceThreshold(float t) {
        line_distance_thresh_sq_ = t * t;
    }

     void setBackFaceCulling(bool v) { back_face_culling_ = v ; }

private:

    bool intersect(const Ray &ray, const GeometryPtr &geom, std::vector<RayCastResult> &results);

private:
    float point_distance_thresh_sq_ = 0.001 ;
    float line_distance_thresh_sq_ = 0.001 ;

    bool back_face_culling_ = true ;


};

}

#endif
