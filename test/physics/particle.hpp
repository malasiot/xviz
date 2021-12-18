#ifndef XVIZ_PHYSICS_PARTICLE_HPP
#define XVIZ_PHYSICS_PARTICLE_HPP

#include <vector>
#include <Eigen/Geometry>

namespace xviz {

struct Particle {
    float mass_, w_ ;
    Eigen::Vector3f x0_ ;
    Eigen::Vector3f x_ ;
    Eigen::Vector3f v_ ;
    Eigen::Vector3f p_ ;
    Eigen::Vector3f f_ ;
    Eigen::Vector3f r_ ;
};

}

#endif
