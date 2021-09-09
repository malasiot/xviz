#ifndef CLSIM_PHYSICS_WORLD_HPP
#define CLSIM_PHYSICS_WORLD_HPP

#include <vector>
#include <memory>

#include <Eigen/Geometry>

namespace clsim {

class Cloth ;
class CollisionObject ;

class World {

public:

    std::unique_ptr<Cloth> cloth_ ;
    std::vector<std::unique_ptr<CollisionObject>> objects_ ;
    Eigen::Vector3f g_ = { 0.0, -10, 0.0 } ;
};

}

#endif
