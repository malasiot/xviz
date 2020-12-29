#ifndef XVIZ_PHYSICS_CONSTRAINTS_HPP
#define XVIZ_PHYSICS_CONSTRAINTS_HPP

#include <bullet/BulletDynamics/btBulletDynamicsCommon.h>

#include <xviz/physics/rigid_body.hpp>

namespace xviz {

class Constraint {
public:
    btTypedConstraint *handle() const { return handle_.get() ; }
protected:

    std::shared_ptr<btTypedConstraint> handle_ ;
};

class Point2PointConstraint: public Constraint {

public:
    Point2PointConstraint(const RigidBodyPtr &b1, const RigidBodyPtr &b2, const Eigen::Vector3f &pivot1, const Eigen::Vector3f &pivot2) ;
};

} // namespace xviz


#endif
