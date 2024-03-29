#ifndef XVIZ_PHYSICS_SOLVER_HPP
#define XVIZ_PHYSICS_SOLVER_HPP

#include <vector>
#include <Eigen/Geometry>

namespace xviz {

class World ;
class Particle ;

class ContactConstraint {
public:
    ContactConstraint(Particle &p, const Eigen::Vector3f &c, const Eigen::Vector3f &n) ;

    void resolve(float dt) ;


    Particle &p_ ;
    Eigen::Vector3f c_, n_ ;
};

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
    void processCollisions() ;
    void resolveContacts(float dt) ;

    World &world_ ;

    float k_damping_ = 0.00125f;
    unsigned int num_iterations_ = 2 ;
    std::vector<ContactConstraint> contacts_ ;
};

}

#endif
