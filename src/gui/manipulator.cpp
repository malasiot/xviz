#include <xviz/gui/manipulator.hpp>
#include <xviz/scene/node_helpers.hpp>
#include "renderer/util.hpp"
#include <QMouseEvent>
#include <QDebug>

using namespace Eigen ;
using namespace std ;

namespace xviz {

void Manipulator::setCamera(const CameraPtr &cam) {
    camera_ = cam ;
    onCameraUpdated() ;
}


bool CompositeManipulator::onMousePressed(QMouseEvent *event) {
    float mint = std::numeric_limits<float>::max() ;
    Ray ray = camera_->getRay(event->x(), event->y()) ;

    for( const auto &m: components_ ) {
        float t ;
        if ( m->hitTest(ray, t) && t < mint ) {
            current_ = m ;
            mint = t ;
        }
    }

    if ( current_ )  {
        current_->onMousePressed(event) ;
        return true ;
    }
    return false ;
    /*
    for( const auto &m: components_ ) {
        if ( m->onMousePressed(event) ) return true ;
    }
    return false ;
    */
}

bool CompositeManipulator::onMouseReleased(QMouseEvent *event) {
    current_ = nullptr ;
    for( const auto &m: components_ ) {
        if ( m->onMouseReleased(event) ) return true ;
    }
    return false ;
}

bool CompositeManipulator::onMouseMoved(QMouseEvent *event) {
    if ( current_ ) {
        current_->onMouseMoved(event) ;
        return true ;
    } else {

        float mint = std::numeric_limits<float>::max() ;
        Ray ray = camera_->getRay(event->x(), event->y()) ;
        ManipulatorPtr hit ;
        for( const auto &m: components_ ) {
            float t ;
            if ( m->hitTest(ray, t) && t < mint ) {
                hit = m ;
                mint = t ;
            }
        }

        if ( selected_ && selected_ != hit ) selected_->setSelected(false) ;
        selected_ = hit ;
        if ( selected_ ) {
            selected_->setSelected(true) ;
            return true ;
        }
    }
    return false ;
}

void CompositeManipulator::onCameraUpdated()
{
    for( const auto &m: components_ ) {
        m->onCameraUpdated();
    }
}

void CompositeManipulator::setCamera(const CameraPtr &cam)
{
    camera_ = cam ;
    for( const auto &m: components_ ) {
        m->setCamera(cam);
    }
}

void CompositeManipulator::setCallback(Callback c)
{
    for( const auto &m: components_ ) {
        m->setCallback(c);
    }
}

void CompositeManipulator::addComponent(const ManipulatorPtr &m)
{
    components_.push_back(m) ;
    addChild(m);
}

namespace impl {

bool computeRayProjectionOnLine(const Eigen::Vector3f &pA, const Eigen::Vector3f &pB, // line segment
                                const Eigen::Vector3f &o, const Eigen::Vector3f &v, // ray
                                Eigen::Vector3f &p, float &d, float &s )
{

    Vector3f ab  = pB - pA ;
    float len = ab.norm() ;
    Vector3f d1 = ab / len ;

    const auto &p1 =  pA ;
    const auto &p2 = o ;
    const auto d2 = v.normalized() ;

    auto n = d1.cross(d2) ;
    auto n2 = d2.cross(n) ;
    auto n1 = d1.cross(n) ;

    float denom1 = d1.dot(n2) ;
    float denom2 = d2.dot(n1) ;

    if ( fabs(denom1) < std::numeric_limits<float>::min() ) return false ;

    s = (p2 - p1).dot(n2) / denom1 ;
    float t = (p1 - p2).dot(n1) / denom2 ;


    p = p1 + s * d1 ;
    auto q = p2 + t * d2 ;

    d = ( q - p ).norm();

    //    qDebug() << 'D' << d << p.x() <<  p.y() << p.z() << s;

    return true ;
}
}

TransformGizmo::TransformGizmo(const CameraPtr &cam, float radius, const Node::NodePtr &node): Manipulator(node)
{
    setCamera(cam) ;
    ray_caster_.setBackFaceCulling(false) ;

    createAxisTranslationNode(components_[TX], radius, {1, 0, 0}, {1, 0, 0, 1}) ;
    createAxisTranslationNode(components_[TY], radius, {0, 1, 0}, {0, 1, 0, 1}) ;
    createAxisTranslationNode(components_[TZ], radius, {0, 0, 1}, {0, 0, 1, 1}) ;

    createPlaneTranslationNode(components_[TYZ], radius/4, {1, 0, 0}, {1, 0, 0, 0.5}) ;
    createPlaneTranslationNode(components_[TXZ], radius/4, {0, 1, 0}, {0, 1, 0, 0.5}) ;
    createPlaneTranslationNode(components_[TXY], radius/4, {0, 0, 1}, {0, 0, 1, 0.5}) ;

    createRotateAxisNode(components_[RX], radius, {1, 0, 0}, {1, 0, 0, 1}) ;
    createRotateAxisNode(components_[RY], radius, {0, 1, 0}, {0, 1, 0, 1}) ;
    createRotateAxisNode(components_[RZ], radius, {0, 0, 1}, {0, 0, 1, 1}) ;
}

void TransformGizmo::createAxisTranslationNode(TransformGizmo::Component &c, float rad, const Vector3f &axis, const Vector4f &clr)
{
    MaterialPtr &mat = c.mat_ ;
    mat.reset(new ConstantMaterial(clr)) ;
    mat->enableDepthTest(false) ;

    c.clr_ = clr ;

    NodePtr root(new Node) ;

    float len = 2 * rad ;

    GeometryPtr cone_geom(new Geometry(Geometry::createSolidCone(len * 0.025f, len * 0.1f, 10, 10))) ;

    GeometryPtr cyl_geom(new Geometry(Geometry::createSolidCylinder(len * 0.01f, len, 10, 10)));

    GeometryPtr picking_geom(new Geometry(Geometry::createSolidCylinder(len * 0.05f, len + 0.1f * len, 10, 10))) ;

    Isometry3f linetr ;
    linetr.setIdentity() ;
    linetr.linear() = rotationBetween({0, 0, 1}, axis) ;

    NodePtr line_node(new Node) ;
    line_node->addDrawable(cyl_geom, mat) ;
    line_node->setTransform(linetr) ;

    NodePtr picking_node(new Node) ;
    picking_node->addDrawable(picking_geom, mat) ;
    picking_node->setTransform(linetr) ;
    picking_node->setVisible(false) ;

    NodePtr left_cone(new Node) ;
    left_cone->addDrawable(cone_geom, mat) ;
    left_cone->transform().linear() = rotationBetween({0, 0, 1}, Vector3f(-axis)) ;
    left_cone->transform().translation() = - axis * rad;

    NodePtr right_cone(new Node) ;
    right_cone->addDrawable(cone_geom, mat) ;
    right_cone->transform().linear() = rotationBetween({0, 0, 1}, axis) ;
    right_cone->transform().translation() = axis * rad;

    root->addChild(line_node) ;
    root->addChild(left_cone) ;
    root->addChild(right_cone) ;
    root->addChild(picking_node) ;

    c.node_ = root ;
    addChild(root) ;
}

void TransformGizmo::createPlaneTranslationNode(TransformGizmo::Component &c, float sz, const Vector3f &axis, const Vector4f &clr)
{
    MaterialPtr &mat = c.mat_ ;
    mat.reset(new ConstantMaterial(clr)) ;
    mat->enableDepthTest(false) ;
    mat->setSide(Material::Side::Both) ;

    c.clr_ = clr ;

    NodePtr root(new Node) ;

    GeometryPtr plane_geom(new Geometry(Geometry::makePlane(sz, sz, 2, 2))) ;

    Isometry3f planetr ;
    planetr.setIdentity() ;
    planetr.linear() = rotationBetween({0, 1, 0}, axis) ;
    planetr.translation() = planetr.linear().inverse() * Vector3f{sz, 0, sz} ;
    /*
    v0_ = planetr_ * Vector3f{-sz/2, 0, -sz/2} ;
    v1_ = planetr_ * Vector3f{sz/2, 0, -sz/2} ;
    v2_ = planetr_ * Vector3f{sz/2, 0, sz/2} ;
    v3_ = planetr_ * Vector3f{-sz/2, 0, sz/2} ;
*/
    NodePtr plane_node(new Node) ;
    plane_node->addDrawable(plane_geom, mat) ;
    plane_node->setTransform(planetr) ;

    root->addChild(plane_node) ;
    c.node_ = root ;
    addChild(root) ;
}

void TransformGizmo::createRotateAxisNode(TransformGizmo::Component &c, float radius, const Vector3f &axis, const Vector4f &clr)
{
    MaterialPtr &mat = c.mat_ ;
    mat.reset(new ConstantMaterial(clr)) ;
    mat->enableDepthTest(false) ;
    mat->setSide(Material::Side::Both);

    c.clr_ = clr ;

    NodePtr root(new Node) ;

    GeometryPtr circle_geom(new Geometry(std::move(Geometry::createSolidTorus(radius, 0.025 *radius, 21, 65)))) ;

    NodePtr circle_node(new Node) ;
    circle_node->addDrawable(circle_geom, mat) ;
    circle_node->transform().linear() = rotationBetween({0, 0, 1}, axis) ;

    c.node_ = root ;
    root->addChild(circle_node) ;
    addChild(root) ;
}

void TransformGizmo::translateAxis(const Vector3f &axis, const Vector3f &start_pt, const Ray &ray)
{

}

bool TransformGizmo::onMousePressed(QMouseEvent *event)
{
    RayCastResult rc ;
    int c = hitTest(event, rc) ;
    if ( c == -1 ) return false ;

    start_drag_ = rc.pt_ ;
    start_tr_ = transform_node_->transform() ;

    dragging_ = c ;
    return true ;
}

bool TransformGizmo::onMouseReleased(QMouseEvent *event) {
    dragging_ = false ;

    return false ;
}

bool TransformGizmo::onMouseMoved(QMouseEvent *event) {

    if ( dragging_ != -1 ) {
        Ray ray = camera_->getRay(event->x(), event->y()) ;
        switch (dragging_) {
        case TX:
            translateAxis({1, 0, 0}, start_drag_, ray) ;
        }

        return true ;
    } else {
        RayCastResult r ;
        int c = hitTest(event, r) ;
        setSelection(c) ;
        return ( c != -1 )  ;
    }

    return false ;
}

void TransformGizmo::setSelection(int c) {
    if ( selected_ != -1 && selected_ != c ) highlight(selected_, false) ;
    selected_ = c ;
    if ( selected_ != -1 ) {
        highlight(selected_, true) ;
    }
}

void TransformGizmo::highlight(int c, bool v)
{
    auto &comp = components_[c] ;
    if ( v )
        comp.setMaterialColor(pick_clr_) ;
    else
        comp.setMaterialColor(comp.clr_) ;
}

void TransformGizmo::onCameraUpdated()
{

}

int TransformGizmo::hitTest(QMouseEvent *event, RayCastResult &res)
{
    Ray ray = camera_->getRay(event->x(), event->y()) ;

    float tmin = std::numeric_limits<float>::max() ;
    int c = -1 ;
    for( uint i=0 ; i<N_COMPONENTS ; i++ ) {
        if ( !components_[i].node_ ) continue ;
        RayCastResult r ;
        if ( ray_caster_.intersect(ray, components_[i].node_, r) && r.t_ < tmin ) {
            tmin = r.t_ ;
            c = i ;
            res = r ;
        }
    }

    return c ;
}

void TransformGizmo::Component::setMaterialColor(const Vector4f &clr)
{
    ConstantMaterial *cm = static_cast<ConstantMaterial *>(mat_.get()) ;
    cm->setColor(clr) ;
}


}
