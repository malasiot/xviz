#ifndef XVIZ_PHYSICS_CLOTH_HPP
#define XVIZ_PHYSICS_CLOTH_HPP

#include <vector>
#include <Eigen/Geometry>

#include "particle.hpp"
#include "constraint.hpp"
#include <memory>


namespace xviz {

class Constraint ;

class Cloth {
public:

    void reset(unsigned int n_iterations) ;

    void getMesh(std::vector<Eigen::Vector3f> &vertices,
                                               std::vector<Eigen::Vector3f> &normals,
                                               std::vector<uint32_t> &indices) ;
protected:
    struct Face {
        Face(uint32_t v0, uint32_t v1, uint32_t v2): v0_(v0), v1_(v1), v2_(v2) {}

        uint32_t v0_, v1_, v2_ ;
        Eigen::Vector3f n_ ;
    } ;

    friend class Solver ;
    friend class World ;

    std::vector<Particle> particles_ ;
    std::vector<DistanceConstraint> distance_constraints_ ;
    std::vector<DihedralConstraint> bending_constraints_ ;
    std::vector<Face> faces_ ;
};

class RectangularPatch: public Cloth {
public:
    enum Anchors {  TopLeft = 0x1, TopRight = 0x2, BottomLeft = 0x4, BottomRight = 0x8,
                    TopEdge = 0x10, BottomEdge = 0x20, LeftEdge = 0x40, RightEdge = 0x80 } ;

    RectangularPatch(float mass, const Eigen::Vector3f &c00, const Eigen::Vector3f &c10, const Eigen::Vector3f &c01,
                unsigned int nvX, unsigned int nvY, unsigned int anchors, float k_stretch, float k_bend);


};

}

#endif
