#include <xviz/physics/constraints.hpp>
#include <xviz/physics/convert.hpp>

namespace xviz {

Point2PointConstraint::Point2PointConstraint(const RigidBodyPtr &b1, const RigidBodyPtr &b2, const Eigen::Vector3f &pivot1, const Eigen::Vector3f &pivot2) {
    handle_.reset(new btPoint2PointConstraint (*b1->handle(), *b2->handle(), eigenVectorToBullet(pivot1), eigenVectorToBullet(pivot2))) ;
}

}
