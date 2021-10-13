#include "constraint.hpp"
#include "particle.hpp"

using namespace Eigen ;

namespace xviz {

const float eps = static_cast<float>(1e-6);

void Constraint::init(unsigned int solver_iterations) {
    kp_ = 1.0f - pow((1.0f - k_), 1.0f/solver_iterations);
    kp_ = std::min(1.0f, kp_) ;
}

DistanceConstraint::DistanceConstraint(Particle &p0, Particle &p1, float rest_length, float k): Constraint(k),
    p0_(p0), p1_(p1), length_(rest_length) {
}

DistanceConstraint::DistanceConstraint(Particle &p0, Particle &p1, float k): Constraint(k),
     p0_(p0), p1_(p1) {
    length_ = (p0.x0_ - p1.x0_).norm() ;
}

void DistanceConstraint::project()
{
    float w = p0_.w_ + p1_.w_ ;
    if ( w == 0.0 ) return ;

    Vector3f &p0 = p0_.p_ ;
    Vector3f &p1 = p1_.p_ ;

    Vector3f n = p1 - p0 ;
    float d = n.norm() ;
    n.normalize();

    Vector3f corr = kp_ * n * ( d - length_ ) / w ;

    if ( p0_.w_ > 0.0 ) p0 += p0_.w_ * corr ;
    if ( p1_.w_ > 0.0 ) p1 += - p1_.w_ * corr ;
}

DihedralConstraint::DihedralConstraint(Particle &p0, Particle &p1, Particle &p2, Particle &p3, float phi0, float k): Constraint(k),
    p0_(p0), p1_(p1), p2_(p2), p3_(p3), phi0_(phi0) {
}

// from https://github.com/InteractiveComputerGraphics/PositionBasedDynamics/blob/master/PositionBasedDynamics/PositionBasedDynamics.cpp
void DihedralConstraint::project()
{
    Vector3f &p0 = p0_.p_ ;
    Vector3f &p1 = p1_.p_ ;
    Vector3f &p2 = p2_.p_ ;
    Vector3f &p3 = p3_.p_ ;

    Vector3f e = p3 - p2 ;
    float  elen = e.norm();

    if ( elen < eps ) return ;

    float ilen = 1.0f / elen;

    Vector3f n1 = ( p2 - p0 ).cross(p3 - p0); n1 /= n1.squaredNorm() ;
    Vector3f n2 = ( p3 - p1 ).cross(p2 - p1); n2 /= n2.squaredNorm() ;

    Vector3f d0 = elen*n1;
    Vector3f d1 = elen*n2;
    Vector3f d2 = (p0-p3).dot(e) * ilen * n1 + (p1-p3).dot(e) * ilen * n2;
    Vector3f d3 = (p2-p0).dot(e) * ilen * n1 + (p2-p1).dot(e) * ilen * n2;

    n1.normalize() ;
    n2.normalize() ;

    float dot = n1.dot(n2);

    if (dot < -1.0) dot = -1.0;
    if (dot >  1.0) dot =  1.0;
    float phi = acos(dot);
    float lambda = p0_.w_  * d0.squaredNorm() +
            p1_.w_ * d1.squaredNorm() +
            p2_.w_ * d2.squaredNorm() +
            p3_.w_ * d3.squaredNorm();

    if (lambda == 0.0) return ;

    // stability
    // 1.5 is the largest magic number I found to be stable in all cases :-)
    //if (stiffness > 0.5 && fabs(phi - b.restAngle) > 1.5)
    //	stiffness = 0.5;

    lambda = (phi - phi0_) / lambda * kp_ ;

    if (n1.cross(n2).dot(e) > 0.0)
        lambda = -lambda;

    Vector3f corr0 = - p0_.w_ * lambda * d0;
    Vector3f corr1 = - p1_.w_ * lambda * d1;
    Vector3f corr2 = - p2_.w_ * lambda * d2;
    Vector3f corr3 = - p3_.w_ * lambda * d3;

    if ( p0_.w_ > 0.0f ) p0 += corr0 ;
    if ( p1_.w_ > 0.0f ) p1 += corr1 ;
    if ( p2_.w_ > 0.0f ) p2 += corr2 ;
    if ( p3_.w_ > 0.0f ) p3 += corr3 ;
}

BendingConstraint::BendingConstraint(Particle &p0, Particle &p1, Particle &p2, Particle &p3, float phi0, float k): Constraint(k),
    p0_(p0), p1_(p1), p2_(p2), p3_(p3), phi0_(phi0) {
}

// bending constraint from PBD paper
void BendingConstraint::project() {

    Vector3f p1 = p0_.p_ ;
    Vector3f p2 = p1_.p_ - p1;
    Vector3f p3 = p2_.p_ - p1;
    Vector3f p4 = p3_.p_ - p1;

    Vector3f n1, n2 ;

    Vector3f p2p3 = p2.cross(p3) ;
    Vector3f p2p4 = p2.cross(p4) ;

    float lenp2p3 = p2p3.norm() ;

    if ( lenp2p3 == 0.0 ) return ;

    float lenp2p4 = p2p4.norm() ;

    if ( lenp2p4 == 0.0 ) return ;

    n1 = p2p3.normalized() ;
    n2 = p2p4.normalized() ;

    float d	= n1.dot(n2) ;
    float phi = acos(d);

    if ( d < -1.0 ) d = -1.0 ;
    else if ( d > 1.0 ) d = 1.0 ;

    //in both case sqrt(1-d*d) will be zero and nothing will be done.
    //0?case, the triangles are facing in the opposite direction, folded together.
    if (d == -1.0 ) {
        phi = M_PI ;  //acos(-1.0) == PI
        if ( phi == phi0_ ) return; //nothing to do

        //in this case one just need to push
        //vertices 1 and 2 in n1 and n2 directions,
        //so the constrain will do the work in second iterations.
        /*
        if (c.p1!=0 && c.p1!=numX )
            tmp_X[c.p3] += n1/100.0f;

          if(c.p2!=0 && c.p2!=numX)
            tmp_X[c.p4] += n2/100.0f;

          return;
        }
        if(d == 1.0){ //180?case, the triangles are planar
            phi = 0.0;  //acos(1.0) == 0.0
            if(phi == phi0[index])
                return; //nothing to do
                */
    }

    float i_d = sqrt(1-(d*d))*(phi-phi0_) ;

    Vector3f p2n1 = p2.cross(n1);
    Vector3f p2n2 = p2.cross(n2);
    Vector3f p3n2 = p3.cross(n2);
    Vector3f p4n1 = p4.cross(n1);
    Vector3f n1p2 = -p2n1;
    Vector3f n2p2 = -p2n2;
    Vector3f n1p3 = n1.cross(p3);
    Vector3f n2p4 = n2.cross(p4);

    Vector3f q3 =  (p2n2 + n1p2*d)/ lenp2p3;
    Vector3f q4 =  (p2n1 + n2p2*d)/ lenp2p4;
    Vector3f q2 =  (-(p3n2 + n1p3*d)/ lenp2p3) - ((p4n1 + n2p4*d)/lenp2p4);

    Vector3f q1 = -q2-q3-q4;

    float q1_len2 = q1.squaredNorm();
    float q2_len2 = q2.squaredNorm() ;
    float q3_len2 = q3.squaredNorm() ;
    float q4_len2 = q4.squaredNorm() ;

    float sum = p0_.w_ * q1_len2 +
            p1_.w_ * q2_len2 +
            p2_.w_ * q3_len2 +
            p3_.w_ * q4_len2 ;

    Vector3f dp1 = -( (p0_.w_ * i_d) / sum )*q1;
    Vector3f dp2 = -( (p1_.w_ * i_d) / sum )*q2;
    Vector3f dp3 = -( (p2_.w_ * i_d) / sum )*q3;
    Vector3f dp4 = -( (p3_.w_ * i_d) / sum )*q4;

    if( p0_.w_ > 0.0)  p0_.p_ += dp1 * kp_ ;
    if( p1_.w_ > 0.0)  p1_.p_ += dp2 * kp_ ;
    if( p2_.w_ > 0.0)  p2_.p_ += dp3 * kp_ ;
    if( p3_.w_ > 0.0)  p3_.p_ += dp4 * kp_ ;

}
}
