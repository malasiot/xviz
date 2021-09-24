#include <xviz/scene/node_helpers.hpp>
#include <xviz/scene/material.hpp>

using namespace Eigen ;
using namespace std ;

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
    GeometryPtr geom(new Geometry(Geometry::createSolidCube(hs)));
    MaterialPtr mat(new PhongMaterial(clr)) ;
    node->addDrawable(geom, mat) ;
    return node ;
}

NodePtr NodeHelpers::makeCylinder(float r, float h, const Eigen::Vector4f &clr)
{
    NodePtr node(new Node) ;
    GeometryPtr geom(new Geometry(Geometry::createSolidCylinder(r, h, 12, 10)));
    MaterialPtr mat(new PhongMaterial(clr)) ;
    node->addDrawable(geom, mat) ;
    return node ;
}

NodePtr NodeHelpers::makeSphere(float r, const Eigen::Vector4f &clr)
{
    NodePtr node(new Node) ;
    GeometryPtr geom(new Geometry(Geometry::createSolidSphere(r, 12, 10)));
    MaterialPtr mat(new PhongMaterial(clr)) ;
    node->addDrawable(geom, mat) ;
    return node ;
}

NodePtr NodeHelpers::makeArc(const Vector3f& center, const Vector3f& normal, const Vector3f& axis, float radiusA, float radiusB,
                   float minAngle, float maxAngle, const Vector3f& color, bool drawSect, float stepDegrees)
{
    vector<Vector3f> ls ;

    Vector3f vy = normal.cross(axis);
    vy.normalize() ;
    Vector3f vx = vy.cross(normal) ;

    float step = stepDegrees * M_PI/180.0;
    int nSteps = (int)fabs((maxAngle - minAngle) / step);
    if ( nSteps == 0 ) nSteps = 1;
    Vector3f prev = center + radiusA * vx * cos(minAngle) + radiusB * vy * sin(minAngle);

    if (drawSect)
        ls.emplace_back(center) ;

    ls.emplace_back(prev) ;

    for ( int i = 1; i <= nSteps; i++ )
    {
        float angle = minAngle + (maxAngle - minAngle) * i / float(nSteps);
        Vector3f next = center + radiusA * vx * cos(angle) + radiusB * vy * sin(angle);
        ls.emplace_back(next) ;
    }

    if (drawSect)
        ls.emplace_back(center) ;

    return makeLineString(ls, color) ;
}

NodePtr NodeHelpers::makeCircle(const Vector3f &center, const Vector3f &normal, float radius, const Vector3f &color) {
    Vector3f axis = ( fabs(normal.y()) < std::numeric_limits<float>::min() ) ? Vector3f(normal.z(), 0, -normal.x()) : Vector3f(normal.y(), -normal.x(), normal.z()) ;
    return makeArc(center, normal, axis, radius, radius, 0, 2*M_PI, color, false, M_PI/36) ;
}

NodePtr NodeHelpers::makeLineString(const std::vector<Vector3f> &pts, const Vector3f &clr)
{
    GeometryPtr m(new Geometry(Geometry::Lines)) ;
    m->vertices() = pts ;
    m->colors().resize(pts.size(), clr) ;

    NodePtr node(new Node) ;
    MaterialPtr mat(new PerVertexColorMaterial()) ;
    node->addDrawable(m, mat) ;
    return node ;
}


}
