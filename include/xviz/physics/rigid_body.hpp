#ifndef XVIZ_PHYSICS_RIGID_BODY_HPP
#define XVIZ_PHYSICS_RIGID_BODY_HPP

#include <memory>


#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <xviz/physics/collision.hpp>
#include <xviz/physics/convert.hpp>

namespace xviz {

struct RigidBodyData ;

class RigidBody: public CollisionObject {
public :
    // dynamic body with given inertia
    RigidBody(btScalar mass, btMotionState *ms, const CollisionShapePtr &shape, const Eigen::Vector3f &localInertia) ;

    // dynamic body with intertia computed by collision shape and mass
    RigidBody(btScalar mass, btMotionState *ms, const CollisionShapePtr &shape) ;

    // static body
    RigidBody(const CollisionShapePtr &shape, const Eigen::Affine3f &tr) ;

    btRigidBody *handle() const;

    void setName(const std::string &name);

    std::string getName() const override {
        return name_ ;
    }

    Eigen::Isometry3f getWorldTransform() const override ;
private:

    friend class PhysicsWorld ;

    std::string name_ ;
    std::unique_ptr<btRigidBody> handle_ ;
    CollisionShapePtr collision_shape_ ;
    std::unique_ptr<btMotionState> motion_state_ ;
};

using RigidBodyPtr = std::shared_ptr<RigidBody> ;

} // namespace xviz


#endif
