#include <clsim/scene/raycaster.hpp>
#include <clsim/scene/scene.hpp>
#include <clsim/scene/geometry.hpp>
#include <clsim/scene/detail/octree.hpp>
#include <clsim/scene/detail/intersect.hpp>

#include <iostream>

using namespace Eigen ;
using namespace std ;

namespace clsim {

RayCaster::~RayCaster()
{

}

RayCaster::RayCaster(const ScenePtr &scene): scene_(scene)
{
    for( const Geometry *geom: scene_->geometries() ) {
        if ( !geom->hasCheapIntersectionTest() ) {
            Vector3f bmin, bmax ;
            geom->computeBoundingBox(bmin, bmax);
            std::unique_ptr<detail::AABB> box(new detail::AABB(bmin, bmax)) ;
            boxes_.emplace(geom, std::move(box)) ;
        }
    }

}

bool RayCaster::intersect(const Ray &tr, const GeometryPtr &geom, RayCastResult &result, float &mint) {
    auto it = boxes_.find(geom.get()) ;
    if ( it != boxes_.end() ) { // quick test with AABB
        const auto &box = (*it).second ;
        float t ;
        if ( !rayIntersectsAABB(tr, *box, t) ) return false ;
    }

    if ( geom->ptype() == Geometry::Triangles ) { // triangle mesh ray intersection
        if ( geom->hasCheapIntersectionTest() ) {
            float t ;
            if ( geom->intersect(tr, t) && t < mint ) {
                mint = t ;
                result.t_ = t ;
                // call intersection to get the hit triangle
                geom->intersectTriangles(tr, result.triangle_idx_, result.t_) ;
                return true;
            }
        } else { // expensive test
            auto it = octrees_.find(geom.get()) ;
            if ( it != octrees_.end() ) { // test if octree is available
                const auto &octree = (*it).second ;
                uint32_t tindex[3] ;
                float t ;
                if ( octree->intersect(tr, tindex, t) && t < mint ) {
                    mint = t ;
                    result.t_ = t ;
                    result.triangle_idx_[0] = tindex[0] ;
                    result.triangle_idx_[1] = tindex[1] ;
                    result.triangle_idx_[2] = tindex[2] ;
                    return true ;
                }
            } else { // no octree expensive test
                float t ;
                uint32_t tindex[3] ;
                if ( geom->intersectTriangles(tr, tindex, t) && t < mint ) {
                    mint = t ;
                    result.t_ = t ;
                    result.triangle_idx_[0] = tindex[0] ;
                    result.triangle_idx_[1] = tindex[1] ;
                    result.triangle_idx_[2] = tindex[2] ;
                    return true ;
                }
            }
        }
    } else if ( geom->ptype() == Geometry::Points ) {
        auto vertices = geom->vertices() ;
        bool found = false ;
        for( uint i=0 ; i<vertices.size() ; i++ ) {
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

}

bool RayCaster::intersect(const Ray &ray, RayCastResult &result)
{
    float mint = std::numeric_limits<float>::max() ;
    bool found = false ;


    for( NodePtr node: scene_->getNodesRecursive() ) {
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

void RayCaster::buildOctrees()
{
    for ( const Geometry *geom: scene_->geometries() ) {
        if ( !geom->hasCheapIntersectionTest() ) {
            if ( geom->ptype() == Geometry::Triangles ) {
                std::unique_ptr<detail::Octree> tree(new detail::Octree(5)) ;
                tree->create(*geom) ;
                octrees_.emplace(geom, std::move(tree)) ;
            }
        }
    }
}



}
