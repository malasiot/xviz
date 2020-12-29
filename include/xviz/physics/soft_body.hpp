#ifndef CVX_VIZ_PHYSICS_SOFT_BODY_HPP
#define CVX_VIZ_PHYSICS_SOFT_BODY_HPP

#include <memory>


#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletSoftBody/btSoftBody.h>

#include <xviz/physics/collision.hpp>
#include <xviz/physics/convert.hpp>

#include <xviz/scene/geometry.hpp>

namespace xviz {

class PhysicsWorld ;

class SoftBody: public CollisionObject {
protected:
    SoftBody() = default  ;
public :

    ~SoftBody() = default ;

    btSoftBody *handle() const { return handle_.get(); }

    void setName(const std::string &name) { name_ = name; }

    std::string getName() const override {
        return name_ ;
    }

    virtual void getVisualGeometry(std::vector<Eigen::Vector3f> &vertices,
                                   std::vector<Eigen::Vector3f> &normals,
                                   std::vector<uint32_t> &indices) const = 0 ;

    Eigen::Isometry3f getWorldTransform() const override { return tr_ ; }
protected:

    friend class PhysicsWorld ;

    std::string name_ ;
    std::unique_ptr<btSoftBody> handle_ ;
    Eigen::Isometry3f tr_ ;
};

using SoftBodyPtr = std::shared_ptr<SoftBody> ;

class SoftPatch2D: public SoftBody {
public:
    SoftPatch2D(PhysicsWorld &w, const float size, uint numX, uint numY, uint flags) ;
    SoftPatch2D(PhysicsWorld &physics, const Eigen::Vector3f &c00, const Eigen::Vector3f &c10, const Eigen::Vector3f &c01,
                uint nvX, uint nvY, uint flags, bool gendiags);

    virtual void getVisualGeometry(std::vector<Eigen::Vector3f> &vertices,
                                   std::vector<Eigen::Vector3f> &normals,
                                   std::vector<uint32_t> &indices) const override ;
};
} // namespace xviz


#endif
