#ifndef XVIZ_PHYSICS_WORLD_HPP
#define XVIZ_PHYSICS_WORLD_HPP

#include <vector>
#include <memory>

#include <Eigen/Geometry>
#include <xviz/scene/scene_fwd.hpp>

namespace xviz {

class Cloth ;
class CollisionObject ;
class Solver ;

class World {

public:
    World() ;
    ~World() ;
    void setCloth(Cloth *cloth);
    void setSolver(Solver *solver) ;
    void addCollisionObject(CollisionObject *obj);

    void resetSimulation() ;
    void stepSimulation(float t) ;

    Cloth *cloth() const { return cloth_.get() ; }
    const Eigen::Vector3f &gravity() const { return g_ ; }
    const std::vector<std::unique_ptr<CollisionObject>> &collisionObjects() const { return objects_ ; }

    NodePtr getVisual() const ;

protected:

    void updateVisuals() ;

    std::unique_ptr<Cloth> cloth_ ;
    std::unique_ptr<Solver> solver_ ;
    std::vector<std::unique_ptr<CollisionObject>> objects_ ;

    Eigen::Vector3f g_ = { 0.0, -10, 0.0 } ;
    NodePtr cloth_visual_, collision_visual_ ;
};

}

#endif
