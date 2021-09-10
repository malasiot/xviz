#ifndef CLSIM_PHYSICS_WORLD_HPP
#define CLSIM_PHYSICS_WORLD_HPP

#include <vector>
#include <memory>

#include <Eigen/Geometry>
#include <clsim/scene/scene_fwd.hpp>

namespace clsim {

class Cloth ;
class CollisionObject ;
class Solver ;

class World {

public:

    void setCloth(Cloth *cloth);
    void setSolver(Solver *solver) ;

    void resetSimulation() ;
    void stepSimulation(float t) ;

    Cloth *cloth() const { return cloth_.get() ; }
    const Eigen::Vector3f &gravity() const { return g_ ; }

    NodePtr getVisual() const { return cloth_visual_ ; }

protected:

    void updateVisuals() ;

    std::unique_ptr<Cloth> cloth_ ;
    std::unique_ptr<Solver> solver_ ;
  //  std::vector<std::unique_ptr<CollisionObject>> objects_ ;
    Eigen::Vector3f g_ = { 0.0, -10, 0.0 } ;
    NodePtr cloth_visual_ ;
};

}

#endif
