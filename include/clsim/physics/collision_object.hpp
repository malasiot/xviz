#ifndef CLSIM_PHYSICS_COLLISION_OBJECT_HPP
#define CLSIM_PHYSICS_COLLISION_OBJECT_HPP

#include <clsim/scene/camera.hpp>
#include <clsim/scene/scene_fwd.hpp>

namespace clsim {

namespace impl {
class BVH ;
}

class CollisionObject {
public:
    CollisionObject(const std::vector<Eigen::Vector3f> &vertices, const std::vector<uint32_t> &indices,
                    const std::vector<Eigen::Vector3f> &normals) ;
    CollisionObject(const std::string &path, const Eigen::Isometry3f &mat = Eigen::Isometry3f::Identity()) ;
    ~CollisionObject();

    NodePtr getVisual() const { return visual_ ; }

    bool intersect(const Ray &ray, Eigen::Vector3f &v, Eigen::Vector3f &n, float &t) ;
private:
    void makeVisual(const std::vector<Eigen::Vector3f> &vertices, const std::vector<uint32_t> &indices, const std::vector<Eigen::Vector3f> &normals);

    std::unique_ptr<impl::BVH> bvh_ ;
    NodePtr visual_ ;
};


}

#endif
