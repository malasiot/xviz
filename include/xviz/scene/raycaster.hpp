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

    void addNode(const NodePtr &node, bool recursive) ;

    bool intersect(const Ray &ray, RayCastResult &result) ;

    void buildOctrees() ;

    void setPointDistanceThreshold(float t) {
        point_distance_thresh_sq_ = t * t;
    }

    void setLineDistanceThreshold(float t) {
        line_distance_thresh_sq_ = t * t;
    }

     void updateBoxes(const NodePtr &n);

     void setBackFaceCulling(bool v) { back_face_culling_ = v ; }

private:
    std::vector<NodePtr> nodes_ ;
    std::map<const Geometry *, std::unique_ptr<detail::Octree>> octrees_ ;
    std::map<const Geometry *, std::unique_ptr<detail::AABB>> boxes_ ;

private:

    bool intersect(const Ray &ray, const NodePtr &node, RayCastResult &res);
    bool intersect(const Ray &ray, const GeometryPtr &geom, RayCastResult &result, float &mint);

private:
    float point_distance_thresh_sq_ = 0.001 ;
    float line_distance_thresh_sq_ = 0.001 ;

    bool back_face_culling_ = true ;

};

}

#endif
