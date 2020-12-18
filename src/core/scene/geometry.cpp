#include <xviz/scene/geometry.hpp>
#include <xviz/scene/mesh.hpp>

#include <iostream>

#include "intersect.hpp"

using namespace std ;

namespace xviz {

using namespace detail ;

bool BoxGeometry::intersect(const Ray &ray, float &t) const {
    AABB box(-half_extents_, half_extents_) ;
    return rayIntersectsAABB(ray, box, t) ;
}

bool SphereGeometry::intersect(const Ray &ray, float &t) const {
    return rayIntersectsSphere(ray, {0, 0, 0}, radius_, t) ;
}


bool MeshGeometry::intersect(const Ray &ray, float &t) const {
    return mesh_->intersect(ray, t) ;
}

MeshGeometry::MeshGeometry(const MeshPtr &mesh) {
    mesh_ = mesh ;
}


void BoxGeometry::makeMesh() {
    mesh_ = Mesh::createSolidCube(halfExtents()) ;
}

void CylinderGeometry::makeMesh() {
    mesh_ = Mesh::createSolidCylinder(radius_, height_, 12, 10) ;
}

void SphereGeometry::makeMesh() {
    mesh_ = Mesh::createSolidSphere(radius_, 12, 10) ;
}

}
