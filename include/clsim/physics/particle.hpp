#ifndef CLSIM_PHYSICS_PARTICLE_HPP
#define CLSIM_PHYSICS_PARTICLE_HPP

#include <vector>
#include <Eigen/Geometry>

namespace clsim { namespace impl {

struct Particle {
    float mass_, inv_mass_ ;
    Eigen::Vector3f x0_ ;
    Eigen::Vector3f x_ ;
    Eigen::Vector3f v_ ;
    Eigen::Vector3f a_ ;
    Eigen::Vector3f x_old_ ;
    Eigen::Vector3f x_last_ ;
};

}}

#endif
