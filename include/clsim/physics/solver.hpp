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

    World &world_ ;
};

}

#endif
