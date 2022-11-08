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

bool RayCaster::intersect(const Ray &ray, const NodePtr &scene, std::vector<RayCastResult> &results) {
    return intersect(ray, scene->getNodesRecursive(), results) ;
}

bool RayCaster::intersect(const Ray &tr, const GeometryPtr &geom, std::vector<RayCastResult> &results) {

    if ( geom->ptype() == Geometry::Triangles ) { // triangle mesh ray intersection
        if ( geom->hasCheapIntersectionTest() ) {
            float t ;
            if ( geom->intersect(tr, t) ) {
                // call intersection to get the hit triangle

                vector<Geometry::RayTriangleHit> rh ;
                geom->intersectTriangles(tr, rh, back_face_culling_) ;
                for( const auto &r: rh ) {
                    RayCastResult result ;
                    result.t_ = r.t_ ;
                    for( size_t i=0 ; i<3 ; i++ ) result.triangle_idx_[i] = r.tidx_[i] ;
                    results.emplace_back(std::move(result)) ;
                }
            }
        } else { // expensive test
            float t ;
            const auto box = geom->getBoundingBox() ;
            if ( !rayIntersectsAABB(tr, box, t) ) return false ;

            // expensive test
            vector<Geometry::RayTriangleHit> rh ;
            if ( geom->intersectTriangles(tr, rh, back_face_culling_) ) {
                for( const auto &r: rh ) {
                    RayCastResult result ;
                    result.t_ = r.t_ ;
                    for( size_t i=0 ; i<3 ; i++ ) result.triangle_idx_[i] = r.tidx_[i] ;
                    results.emplace_back(std::move(result)) ;
                }
            }
        }
    } else if ( geom->ptype() == Geometry::Points ) {
        auto vertices = geom->vertices() ;

        for( unsigned int i=0 ; i<vertices.size() ; i++ ) {
            const Vector3f &v = vertices[i] ;
            float t ;
            if ( detail::rayIntersectsPoint(tr, v, point_distance_thresh_sq_, t)  ) {
                RayCastResult result ;
                result.t_ = t ;
                result.point_idx_ = i ;
                results.emplace_back(std::move(result)) ;
            }
        }

    } else if ( geom->ptype() == Geometry::Lines ) {
        vector<Geometry::RayLineHit> hits ;
        if ( geom->intersectLines(tr, hits, line_distance_thresh_sq_)  ) {
            for( const auto &r: hits ) {
                RayCastResult result ;
                result.t_ = r.t_ ;
                for( size_t i=0 ; i<2 ; i++ ) result.line_idx_[i] = r.tidx_[i] ;
                results.emplace_back(std::move(result)) ;
            }
        }
    }

    return !results.empty() ;
}

bool RayCaster::intersect(const Ray &ray, const std::vector<NodePtr> &nodes, vector<RayCastResult> &results)
{
    for( NodePtr node: nodes ) {
        Affine3f tf = node->globalTransform().inverse() ;
        Ray tr(ray, tf) ; // ray transform to local coordinate system

        for(  Drawable &dr: node->drawables() ) {
            GeometryPtr geom = dr.geometry() ;

            vector<RayCastResult> lres ;
            if ( intersect(tr, geom, lres) ) {
                for( auto &l: lres ) {
                    l.drawable_ = &dr ;
                    l.node_ = node ;
                    results.emplace_back(l) ;
                }
            }


        }
    } ;

    return !results.empty() ;
}

bool RayCaster::intersectOne(const Ray &ray, const std::vector<NodePtr> &nodes, RayCastResult &result) {
    vector<RayCastResult> results ;
    if ( intersect(ray, nodes, results) ) {
         result = results[0] ;
         return true ;
    } else
        return false ;
}

bool RayCaster::intersectOne(const Ray &ray, const NodePtr &scene, RayCastResult &result) {
    return intersectOne(ray, scene->getNodesRecursive(), result) ;
}

}
