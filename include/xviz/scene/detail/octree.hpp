#ifndef XVIZ_SCENE_OCTREE_HPP
#define XVIZ_SCENE_OCTREE_HPP

#include <Eigen/Core>
#include <vector>

#include <xviz/scene/camera.hpp>

using Eigen::Vector3f ;
using std::vector ;


namespace xviz {

class Geometry ;

namespace detail {

struct OctreeNode
{
    struct Triangle {
        Triangle(uint32_t idx0, uint32_t idx1, uint32_t idx2)
             { tidx_[0] = idx0 ; tidx_[1] = idx1 ; tidx_[2] = idx2 ; }

        Triangle(const uint32_t tidx[3])
             { tidx_[0] = tidx[0] ; tidx_[1] = tidx[1] ; tidx_[2] = tidx[2] ; }
        uint32_t tidx_[3] ;
    };

    vector<Triangle> triangles_;

    OctreeNode* children_[8] = { nullptr } ;
    bool is_leaf_ = true ;

    OctreeNode() = default ;
    ~OctreeNode();
};

class Octree
{
public:

    Octree() = default ;
    Octree( uint max_depth = 5, uint max_count = 100);

    ~Octree() { delete root_; }

    void create(const Geometry &Mesh) ;

    void insert(const Geometry &m, const uint32_t tindex[2], const Vector3f &v0, const Vector3f &v1, const Vector3f &v2);

    bool intersect(const Ray &ray, uint32_t tindex[3], float &t);

private:
    void insertTriangle( const Geometry &m, OctreeNode *node, const Vector3f &center, const Vector3f &hs, uint depth,
                         const uint32_t tindex[3], const Vector3f &v0, const Vector3f &v1, const Vector3f &v2);

    bool intersect(OctreeNode *node, const Ray &r, const Vector3f &center, const Vector3f &hs, uint32_t tindex[3], float &mint);

    void getTriangleVertices(const Geometry &Mesh, const uint32_t tindex[3], Vector3f &v0, Vector3f &v1, Vector3f &v2);

    static const Vector3f offsets_[8] ;

    Vector3f center_, hs_ ;

    uint max_depth_ = 5, max_count_ = 100 ;
    OctreeNode* root_;
    const Geometry *mesh_ = nullptr;


};

}}

#endif
