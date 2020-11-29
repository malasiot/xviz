#include <xviz/scene/scene.hpp>
#include <xviz/scene/node.hpp>
#include <xviz/scene/mesh.hpp>

using namespace Eigen ;
using namespace std ;

namespace xviz {

Vector3f Scene::geomCenter() const {
    Vector3f center {0, 0, 0} ;
    uint count = 0 ;

    visitNodes([&](const Node &node){
        Affine3f tf = node.globalTransform() ;
        for ( const auto &d: node.drawables() ) {
            MeshPtr mesh = d->geometry() ;

            for( const Vector3f &v: mesh->vertices() ) {
                Vector3f p = tf * v ;
                center += p ;
                ++count ;
            }
        }
    });

    if ( count != 0 ) center /= count ;
}

float Scene::geomRadius(const Vector3f &center) const {
    float max_dist = 0.0 ;

    visitNodes([&](const Node &node){
        Affine3f tf = node.globalTransform() ;
        for ( const auto &d: node.drawables() ) {
            MeshPtr mesh = d->geometry() ;

            for( const Vector3f &v: mesh->vertices() ) {
                Vector3f p = tf * v ;
                float dist = (p - center).squaredNorm() ;
                max_dist = std::max(max_dist, dist) ;
            }
        }
    });

    return sqrt(max_dist) ;
}







}
