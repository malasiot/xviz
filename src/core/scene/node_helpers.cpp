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

NodePtr NodeHelpers::makeBox(const Eigen::Vector3f &hs, const Eigen::Vector4f &clr) {
    NodePtr node(new Node) ;
    GeometryPtr geom(new Geometry(std::move(Geometry::createSolidCube(hs))));
    MaterialPtr mat(new PhongMaterial(clr)) ;
    node->addDrawable(geom, mat) ;
    return node ;
}

NodePtr NodeHelpers::makeCylinder(float r, float h, const Eigen::Vector4f &clr)
{
    NodePtr node(new Node) ;
    GeometryPtr geom(new Geometry(std::move(Geometry::createSolidCylinder(r, h, 12, 10))));
    MaterialPtr mat(new PhongMaterial(clr)) ;
    node->addDrawable(geom, mat) ;
    return node ;
}

NodePtr NodeHelpers::makeSphere(float r, const Eigen::Vector4f &clr)
{
    NodePtr node(new Node) ;
    GeometryPtr geom(new Geometry(std::move(Geometry::createSolidSphere(r, 12, 10))));
    MaterialPtr mat(new PhongMaterial(clr)) ;
    node->addDrawable(geom, mat) ;
    return node ;
}

}
