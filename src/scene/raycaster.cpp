#include <xviz/scene/raycaster.hpp>
#include <xviz/scene/scene.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/detail/octree.hpp>
#include <xviz/scene/detail/intersect.hpp>

#include <iostream>

using namespace Eigen ;
using namespace std ;

namespace xviz {

RayCaster::~RayCaster()
{

}


RayCaster::RayCaster() {

}

bool RayCaster::intersect(const Ray &ray, const NodePtr &scene, RayCastResult &result) {
     return intersect(ray, scene->getNodesRecursive(), result) ;
}

bool RayCaster::intersect(const Ray &tr, const GeometryPtr &geom, RayCastResult &result, float &mint) {

    if ( geom->ptype() == Geometry::Triangles ) { // triangle mesh ray intersection
        if ( geom->hasCheapIntersectionTest() ) {
            float t ;
            if ( geom->intersect(tr, t) && t < mint ) {
                mint = t ;
                result.t_ = t ;
                // call intersection to get the hit triangle
                geom->intersectTriangles(tr, result.triangle_idx_, result.t_, back_face_culling_) ;
                return true;
            }
        } else { // expensive test
            float t ;
            const auto box = geom->getBoundingBox() ;
            if ( !rayIntersectsAABB(tr, box, t) ) return false ;

            // expensive test

                uint32_t tindex[3] ;
                if ( geom->intersectTriangles(tr, tindex, t, back_face_culling_) && t < mint ) {
                    mint = t ;
                    result.t_ = t ;
                    result.triangle_idx_[0] = tindex[0] ;
                    result.triangle_idx_[1] = tindex[1] ;
                    result.triangle_idx_[2] = tindex[2] ;
                    return true ;

            }
        }
    } else if ( geom->ptype() == Geometry::Points ) {
        auto vertices = geom->vertices() ;
        bool found = false ;
        for( unsigned int i=0 ; i<vertices.size() ; i++ ) {
            const Vector3f &v = vertices[i] ;
            float t ;
            if ( detail::rayIntersectsPoint(tr, v, point_distance_thresh_sq_, t) && t<mint ) {
                mint = t ;
                result.t_ = t ;
                result.point_idx_ = i ;
                found = true ;
            }
        }
        return found ;
    } else if ( geom->ptype() == Geometry::Lines ) {
        float t ;
        uint32_t tindex[2] ;
        if ( geom->intersectLines(tr, tindex, line_distance_thresh_sq_, t) && t < mint ) {
            mint = t ;
            result.t_ = t ;
            result.line_idx_[0] = tindex[0] ;
            result.line_idx_[1] = tindex[1] ;
            return true ;
        }
    }

    return false ;

}

bool RayCaster::intersect(const Ray &ray, const std::vector<NodePtr> &nodes, RayCastResult &result)
{
    float mint = std::numeric_limits<float>::max() ;
    bool found = false ;


    for( NodePtr node: nodes ) {
        Affine3f tf = node->globalTransform().inverse() ;
        Ray tr(ray, tf) ; // ray transform to local coordinate system

        for(  Drawable &dr: node->drawables() ) {
            GeometryPtr geom = dr.geometry() ;

            if ( intersect(tr, geom, result, mint) ) {
                result.drawable_ = &dr ;
                result.node_ = node ;
                found = true ;
            }


        }
    } ;

    return found ;
}


}
