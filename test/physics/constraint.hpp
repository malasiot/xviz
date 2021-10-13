#ifndef XVIZ_PHYSICS_CONSTRAINT_HPP
#define XVIZ_PHYSICS_CONSTRAINT_HPP

#include <vector>
#include <Eigen/Geometry>

namespace xviz {

class Particle ;

class Constraint {
public:
    Constraint(float k): k_(k) {}

    void init(unsigned int solver_iterations) ;

protected:
    float k_, kp_ ; // k prime
};

class DistanceConstraint: public Constraint {
public:
    DistanceConstraint(Particle &p0, Particle &p1, float rest_length, float k) ;
    DistanceConstraint(Particle &p0, Particle &p1, float k) ;

    void project() ;


protected:
    Particle &p0_, &p1_ ;
    float length_ ;
};

class DihedralConstraint: public Constraint {
public:
    DihedralConstraint(Particle &p0, Particle &p1, Particle &p2, Particle &p3, float phi0, float k) ;

    void project() ;

protected:
    Particle &p0_, &p1_, &p2_, &p3_ ;
    float phi0_ ;
};

class BendingConstraint: public Constraint {
public:
    BendingConstraint(Particle &p0, Particle &p1, Particle &p2, Particle &p3, float phi0, float k) ;

    void project() ;

protected:
    Particle &p0_, &p1_, &p2_, &p3_ ;
    float phi0_ ;
};


}

#endif
