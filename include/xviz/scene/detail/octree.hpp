#ifndef XVIZ_SCENE_OCTREE_HPP
#define XVIZ_SCENE_OCTREE_HPP

#include <Eigen/Core>
#include <vector>

#include <xviz/scene/camera.hpp>

using Eigen::Vector3f ;
using std::vector ;


namespace xviz {

class Mesh ;

namespace detail {

struct OctreeNode
{
    vector<uint32_t> data_;
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

    void create(const Mesh &mesh) ;

    void insert(const Mesh &m, uint32_t tindex, const Vector3f &v0, const Vector3f &v1, const Vector3f &v2);

    bool intersect(const Ray &ray, uint32_t &tindex, float &t);

private:
    void insertTriangle( const Mesh &m, OctreeNode *node, const Vector3f &center, const Vector3f &hs, uint depth,
                         uint32_t tindex, const Vector3f &v0, const Vector3f &v1, const Vector3f &v2);

    bool intersect(OctreeNode *node, const Ray &r, const Vector3f &center, const Vector3f &hs, uint32_t &tindex, float &mint);

    void getTriangleVertices(const Mesh &mesh, uint32_t tindex, Vector3f &v0, Vector3f &v1, Vector3f &v2);

    static const Vector3f offsets_[8] ;

    Vector3f center_, hs_ ;

    uint max_depth_ = 5, max_count_ = 100 ;
    OctreeNode* root_;
    const Mesh *mesh_ = nullptr;


};

}}

#endif
