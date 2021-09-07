#include <clsim/scene/node.hpp>
#include <clsim/scene/geometry.hpp>

#include <unordered_set>

using namespace Eigen ;
using namespace std ;

namespace clsim {


NodePtr Node::findNodeByName(const std::string &name)
{
    if ( name_ == name ) return shared_from_this() ;
    else {
        for( NodePtr n: children_ ) {
            if ( NodePtr res = n->findNodeByName(name) )
                return res ;
        }
        return nullptr ;
    }

}

Vector3f Node::geomCenter() const {
    Vector3f center {0, 0, 0} ;
    uint count = 0 ;

    visit([&](const Node &node){
        Affine3f tf = node.globalTransform() ;
        for ( const auto &d: node.drawables() ) {
            GeometryPtr mesh = d.geometry() ;

            for( const Vector3f &v: mesh->vertices() ) {
                Vector3f p = tf * v ;
                center += p ;
                ++count ;
            }
        }
    });

    if ( count != 0 ) center /= count ;

    return center ;
}

float Node::geomRadius(const Vector3f &center) const {
    float max_dist = 0.0 ;

    visit([&](const Node &node){
        Affine3f tf = node.globalTransform() ;
        for ( const auto &d: node.drawables() ) {
            GeometryPtr mesh = d.geometry() ;

            for( const Vector3f &v: mesh->vertices() ) {
                Vector3f p = tf * v ;
                float dist = (p - center).squaredNorm() ;
                max_dist = std::max(max_dist, dist) ;
            }
        }
    });

    return sqrt(max_dist) ;
}

const std::vector<Material *> Node::materials() const {
    unordered_set<Material *> material_map ;

    visit([&](const Node &n) {
        for( const auto &dr: n.drawables() ) {
            MaterialPtr material = dr.material() ;
            material_map.emplace(material.get()) ;
        }
    });

    return {material_map.begin(), material_map.end()} ;
}

const std::vector<Geometry *> Node::geometries() const {
    unordered_set<Geometry *> geometry_map ;

    visit([&](const Node &n) {
        for( const auto &dr: n.drawables() ) {
            GeometryPtr geom = dr.geometry() ;
            geometry_map.emplace(geom.get()) ;
        }
    });


    return { geometry_map.begin(), geometry_map.end() } ;
}
}
