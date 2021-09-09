#ifndef CLSIM_PHYSICS_SOLVER_HPP
#define CLSIM_PHYSICS_SOLVER_HPP

#include <vector>
#include <Eigen/Geometry>

namespace clsim {

class World ;

class Solver {
public:
    Solver(World &w) ;

    virtual void init() ;
    virtual void step(float h) ;

protected:

    void applyExternalForces(float dt) ;
    void dampVelocities() ;
    void integrate(float dt) ;
    void projectInternalConstraints() ;
    void updateState(float dt) ;

    World &world_ ;

    float k_damping_ = 0.00125f;
    uint num_iterations_ = 4 ;
};

}

#endif
