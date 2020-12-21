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

    Eigen::Vector3f pt_ ; // intersection point
    uint32_t triangle_idx_[3] ; // index of triangle vertices
};

class RayCaster {
public:
    ~RayCaster() ;
    RayCaster(const ScenePtr &scene) ;

    bool intersect(const Ray &ray, RayCastResult &result) ;

    void buildOctrees() ;

private:
    ScenePtr scene_ ;
    std::map<const Geometry *, std::unique_ptr<detail::Octree>> octrees_ ;
    std::map<const Geometry *, std::unique_ptr<detail::AABB>> boxes_ ;

private:
    bool intersect(const Ray &ray, const NodePtr &node, RayCastResult &res);
};

}

#endif
