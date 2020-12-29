#include <xviz/scene/detail/octree.hpp>
#include <xviz/scene/geometry.hpp>

#include <xviz/scene/detail/intersect.hpp>
#include <iostream>

using Eigen::Vector3f ;
using std::vector ;
using namespace std;

namespace xviz { namespace detail {

OctreeNode::~OctreeNode()
{
   for ( int i = 0; i < 8; i++ )
       if (children_[i])
            delete children_[i];
}

const Vector3f Octree::offsets_[8] = { { -0.5, 0.5, -0.5 }, {0.5, 0.5, -0.5}, { -0.5, -0.5, -0.5 }, { 0.5, -0.5, -0.5 },
                                       { -0.5, 0.5, 0.5 }, {0.5, 0.5, 0.5}, { -0.5, -0.5, 0.5 }, { 0.5, -0.5, 0.5 } } ;

void Octree::getTriangleVertices(const Geometry &m, const uint32_t tindex[3], Eigen::Vector3f &v0, Eigen::Vector3f &v1, Eigen::Vector3f &v2) {
    uint i0 = tindex[0] ;
    uint i1 = tindex[1] ;
    uint i2 = tindex[2] ;
    v0 = m.vertices()[i0] ;
    v1 = m.vertices()[i1] ;
    v2 = m.vertices()[i2] ;
}

void Octree::create(const Geometry &mesh) {
  //  vertices_ = Mesh.vertices(), indices_ = Mesh.indices() ;

    mesh_ = &mesh ;

    Vector3f bmin, bmax ;
    mesh.computeBoundingBox(bmin, bmax) ;

    // to handle planar objects (i,e one dimension is zero)
    Vector3f sz = bmax - bmin ;
    float side = std::max(sz.x(), std::max(sz.y(), sz.z()) ) ;
    float hs = side/2.0f ;

    center_ = (bmin + bmax)/2.0 ;
    hs_ = Vector3f( hs, hs, hs ) ;

    uint32_t tidx[3] ;

    for( uint i=0 ; i<mesh.indices().size() ; i+=3 ) {
        tidx[0] = mesh.indices()[i] ;
        tidx[1] = mesh.indices()[i+1] ;
        tidx[2] = mesh.indices()[i+2] ;

        Vector3f v0, v1, v2 ;

        getTriangleVertices(mesh, tidx, v0, v1, v2) ;

        insert(mesh, tidx, v0, v1, v2) ;
    }


}

Octree::Octree(uint max_depth, uint max_count):
    max_depth_(max_depth), max_count_(max_count), root_(new OctreeNode()) {

}

void Octree::insertTriangle(const Geometry &m, OctreeNode *node, const Eigen::Vector3f &center, const Eigen::Vector3f &hs, uint depth, const uint32_t tindex[3],
                            const Eigen::Vector3f &v0, const Eigen::Vector3f &v1, const Eigen::Vector3f &v2) {

    if ( node->is_leaf_ ) {
        if ( node->triangles_.size() < max_count_ || depth == max_depth_ ) { // we have reach deepest layer, just store triangle index
            node->triangles_.emplace_back(tindex) ;
        } else { //
            node->is_leaf_ = false; // make this node internal so that when we reinsert data it will spread to child nodes
            for( const OctreeNode::Triangle &t: node->triangles_ ) {
                Vector3f v0, v1, v2 ;
                getTriangleVertices(m, t.tidx_, v0, v1, v2) ;
                insertTriangle(m, node, center, hs, depth, t.tidx_, v0, v1, v2) ;
            }
            node->triangles_.clear() ;
            insertTriangle(m, node, center, hs, depth, tindex, v0, v1, v2) ;
        }
    } else {
        for( uint i=0 ; i<8 ; i++ ) {
            Vector3f child_center  = center  + Vector3f((offsets_[i].array() * hs.array())) ;

      /*      if ( triangleInsideBox(v0, v1, v2, child_center, hs/2 ) ||
                 triangleIntersectsBox(v0, v1, v2, child_center, hs/2 ) ) {
*/
            if ( !triangleOutsideBox(v0, v1, v2, child_center, hs/2)) { // we use a cheap test here
                if ( node->children_[i] == nullptr )
                    node->children_[i] = new OctreeNode();    // the child intersects the triangle so push it there

                insertTriangle(m, node->children_[i], child_center, hs/2, depth + 1,  tindex, v0, v1, v2) ;
            }
        }
    }
}


void Octree::insert(const Geometry &m, const uint32_t tindex[3], const Eigen::Vector3f &v0, const Eigen::Vector3f &v1, const Eigen::Vector3f &v2) {
    insertTriangle(m, root_, center_, hs_, 0,  tindex, v0, v1, v2) ;
}

bool Octree::intersect(OctreeNode *node, const Ray &r, const Eigen::Vector3f &center, const Eigen::Vector3f &hs, uint32_t tindex[3], float &mint) {

    // first cheap test with octree node AABB

    float t ;
    AABB box(center - hs, center + hs) ;

    if ( !rayIntersectsAABB(r, box, t) ) return false ;  // no need to go deeper

    if ( node->is_leaf_ ) { // reached a leaf node, check ray/triangle intersections

        bool found = false ;

        for( const OctreeNode::Triangle &tr: node->triangles_ ) {

            Vector3f v0, v1, v2 ;
            getTriangleVertices(*mesh_, tr.tidx_, v0, v1, v2) ;

            if ( rayIntersectsTriangle(r, v0, v1, v2, true, t) && t < mint ) {

                tindex[0] = tr.tidx_[0] ;
                tindex[1] = tr.tidx_[1] ;
                tindex[2] = tr.tidx_[2] ;
                found = true ;
                cout << t << ' ' << mint << ' ' << tindex[0] << ' ' << tindex[1] << ' ' << tindex[2] << endl ;
                mint = t ;
            }
        }

        return found ;

    } else { // recurse on child nodes
        bool found = false ;
        for( uint i=0 ; i<8 ; i++ ) {
            if ( node->children_[i] != nullptr ) {
                Vector3f child_center  = center  + Vector3f((offsets_[i].array() * hs.array())) ;

                bool res = intersect(node->children_[i], r, child_center, hs/2, tindex, mint ) ;
                if ( res ) {
                    found = true ;
                }
            }
        }

        return found ;
    }

    return false ;

}

bool Octree::intersect(const Ray &ray, uint tindex[3], float &t) {
    t = std::numeric_limits<float>::max() ;
    return intersect(root_, ray, center_, hs_, tindex, t) ;
}


}}
