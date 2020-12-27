#include <xviz/scene/node_helpers.hpp>

namespace xviz {

NodePtr NodeHelpers::makeAxes(float sz) {
    GeometryPtr m(new Geometry(Geometry::Lines)) ;

    m->vertices() = { { 0, 0, 0 }, {sz, 0, 0}, {0, 0, 0}, {0, sz, 0}, {0, 0, 0}, {0, 0, sz} } ;
    m->colors() = { { 1, 0, 0 }, { 1, 0, 0 }, {0, 1, 0}, {0, 1, 0}, {0, 0, 1}, {0, 0, 1} } ;

    NodePtr node(new Node) ;
    node->addDrawable(m, MaterialPtr(new PerVertexColorMaterial())) ;
    return node ;
}

}
