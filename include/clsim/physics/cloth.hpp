#ifndef CLSIM_PHYSICS_CLOTH_HPP
#define CLSIM_PHYSICS_CLOTH_HPP

#include <vector>
#include <Eigen/Geometry>

#include "particle.hpp"
#include <memory>


namespace clsim { namespace impl {

class Constraint ;

class Cloth {

public:

    std::vector<Particle> particles_ ;
    std::vector<std::unique_ptr<Constraint>> constraints_ ;

};

}}

#endif
