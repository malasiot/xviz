#include <clsim/physics/solver.hpp>
#include <clsim/physics/world.hpp>
#include <clsim/physics/cloth.hpp>

using namespace Eigen ;
using namespace std ;

const float global_dampening = 0.98f;

namespace clsim {

Solver::Solver(World &w): world_(w) {

}

void Solver::init() {
    world_.cloth()->reset(num_iterations_) ;
}

void Solver::step(float dt) {
    applyExternalForces(dt);
    dampVelocities() ;
    integrate(dt) ;
    for( uint iter = 0 ; iter < num_iterations_ ; iter ++ )
        projectInternalConstraints() ;
    updateState(dt) ;
}

void Solver::applyExternalForces(float dt) {
    auto &particles = world_.cloth()->particles_ ;
    for( auto &p: particles ) {
        p.f_ = world_.gravity() ;
        p.v_ *= global_dampening ; //global velocity dampening !!!

        // integrate forces
        p.v_ = p.v_ + p.f_ * p.w_ * dt ;
    }
}

void Solver::dampVelocities() {
    auto &particles = world_.cloth()->particles_ ;

    Vector3f xcm{0, 0, 0}, vcm{0, 0, 0};
    float mass = 0;

    for( auto &p: particles ) {
        //calculate the center of mass's position
        //and velocity for damping calc
        xcm +=  p.x_ * p.mass_;
        vcm +=  p.v_ * p.mass_;
        mass += p.mass_;
    }

    xcm /= mass;
    vcm /= mass;

    Matrix3f I = Matrix3f::Identity();
    Vector3f L = Vector3f::Zero();

    for( auto &p: particles ) {
        Vector3f r = p.x_ - xcm ;
        L += r.cross(p.mass_ * p.v_) ;
        Matrix3f S ;
        S << 0, -r.z(), r.y(),
                r.z(), 0, -r.x(),
                -r.y(), r.x(), 0 ;

        I += S * S.transpose() * p.mass_ ;
    }

    Vector3f omega = I.inverse() * L ;

    //apply center of mass damping

    for( auto &p: particles ) {
        Vector3f r = p.x_ - xcm ;
        Vector3f dv = vcm + omega.cross(r) - p.v_ ;
        p.v_ += k_damping_ * dv ;
    }
}

void Solver::integrate(float dt)
{
    auto &particles = world_.cloth()->particles_ ;

    for(auto &p: particles ) {
        if ( p.w_ <= 0.0 ) p.p_ = p.x_ ;
        else p.p_ = p.x_ + p.v_ * dt ;
    }
}

void Solver::projectInternalConstraints()
{
    auto &cloth = *world_.cloth() ;

    for( auto &c: cloth.distance_constraints_ ) {
        c.project() ;
    }

    for( auto &c: cloth.bending_constraints_ ) {
        c.project() ;
    }
}

void Solver::updateState(float dt) {
    auto &particles = world_.cloth()->particles_ ;

    for( auto &p: particles ) {
        p.v_ = (p.p_ - p.x_)/dt ;
        p.x_ = p.p_ ;
    }
}





}
