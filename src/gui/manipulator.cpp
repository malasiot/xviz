#include <xviz/gui/manipulator.hpp>
#include <xviz/scene/node_helpers.hpp>
#include "renderer/util.hpp"
#include <QMouseEvent>
#include <QDebug>

#include <iostream>
using namespace Eigen ;
using namespace std ;

namespace xviz {

TransformGizmo::TransformGizmo(const CameraPtr &cam, float radius): Manipulator(), camera_(cam)
{
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

    MaterialPtr invisible(new ConstantMaterial({0, 0, 0, 0})) ;
    invisible->enableDepthTest(false) ;

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
    picking_node->addDrawable(picking_geom, invisible) ;
    picking_node->setTransform(linetr) ;


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

    MaterialPtr invisible(new ConstantMaterial({0, 0, 0, 0})) ;
    invisible->enableDepthTest(false) ;

    c.clr_ = clr ;

    NodePtr root(new Node) ;

    GeometryPtr circle_geom(new Geometry(Geometry::createSolidTorus(radius, 0.025 *radius, 21, 65))) ;
    GeometryPtr picking_geom(new Geometry(Geometry::createSolidTorus(radius, 0.05 *radius, 21, 65))) ;

    NodePtr circle_node(new Node) ;
    circle_node->addDrawable(circle_geom, mat) ;
    circle_node->transform().linear() = rotationBetween({0, 0, 1}, axis) ;

    NodePtr picking_node(new Node) ;
    circle_node->addDrawable(picking_geom, invisible) ;
    circle_node->transform().linear() = rotationBetween({0, 0, 1}, axis) ;

    c.node_ = root ;
    root->addChild(circle_node) ;
    root->addChild(picking_node) ;
    addChild(root) ;
}

void TransformGizmo::setTranslation(const Vector3f &v)
{
    position_ = start_pos_ + v ;
    updateTransforms() ;
}

void TransformGizmo::setRotation(const Matrix3f &m)
{
    if ( local_ )
        orientation_ = start_orientation_ * m ;
    else
        orientation_ = m ;

    updateTransforms() ;
}

void TransformGizmo::updateTransforms()
{
    Isometry3f tr = Isometry3f::Identity() ;

    tr.translate(position_) ;
    tr.rotate(orientation_) ;


    transform_node_->transform() = tr ;

    if ( local_ ) {
        transform() = tr ;
    } else {
        transform().translation() = position_ ;
        transform().linear() = Matrix3f::Identity() ;
    }

}



bool intersect_plane(const Vector3f &n, const Vector3f &pos, const Vector3f &orig, const Vector3f &dir, Vector3f &p) {

    Vector3f l = dir.normalized() ;

    // assuming vectors are all normalized
    float denom = n.dot(l) ;
    if (fabs(denom) > 0.1) {
        float t = (pos  - orig).dot(n)/ denom;
         p = orig + t * l ;
        return true ;
    }

    return false ;
}

static bool ray_plane_intersection(const Ray &r, const Vector3f &axis, const Vector3f &pos, const Vector3f &eye, Vector3f &p) {
    const Vector3f plane_tangent = axis.cross(pos - eye);
    const Vector3f plane_normal = axis.cross(plane_tangent);

    return intersect_plane(plane_normal, pos, r.origin(), r.dir(), p) ;
}

static bool ray_plane_intersection(const Ray &r, const Vector3f &axis, const Vector3f &pos, Vector3f &p) {
    return intersect_plane(axis, pos, r.origin(), r.dir(), p) ;
}

static void project_on_axis(Vector3f &pt, const Vector3f &origin, const Vector3f &axis) {
     pt = origin + axis * axis.dot(pt - origin);
}

static float rotation_angle(const Vector3f &pt, const Vector3f &start_drag, const Vector3f &center, const Vector3f &axis) {
    float angle = acos((pt - center).normalized().dot((start_drag - center).normalized())) ;
    Vector3f vc = (pt - center).cross(start_drag - center) ;
    if ( vc.dot(axis) > 0 )
        angle = -angle ;
    return angle ;
}

static Vector3f AXIS_X{1, 0, 0} ;
static Vector3f AXIS_Y{0, 1, 0} ;
static Vector3f AXIS_Z{0, 0, 1} ;

bool TransformGizmo::onMousePressed(QMouseEvent *event)
{
    RayCastResult rc ;
    int c = hitTest(event, rc) ;
    if ( c == -1 ) return false ;

    Ray r = camera_->getRay(event->x(), event->y()) ;

    start_pos_ = position_ ;
    start_orientation_ = orientation_ ;

    Vector3f axis_x = ( local_ ) ? orientation_ * AXIS_X : AXIS_X ;
    Vector3f axis_y = ( local_ ) ? orientation_ * AXIS_Y : AXIS_Y ;
    Vector3f axis_z = ( local_ ) ? orientation_ * AXIS_Z : AXIS_Z ;

    for( int i=0 ; i<N_COMPONENTS ; i++ ) {
        if ( i != c )
            components_[i].node_->setVisible(false) ;
    }

    switch ( c ) {
    case TX:
        ray_plane_intersection(r, axis_x, start_pos_, camera_->eye(), start_drag_); break ;
    case TY:
        ray_plane_intersection(r, axis_y, start_pos_, camera_->eye(), start_drag_); break ;
    case TZ:
        ray_plane_intersection(r, axis_z, start_pos_, camera_->eye(), start_drag_); break ;
    case TYZ:
        ray_plane_intersection(r, axis_x, start_pos_, start_drag_); break ;
    case TXZ:
        ray_plane_intersection(r, axis_y, start_pos_, start_drag_); break ;
    case TXY:
        ray_plane_intersection(r, axis_z, start_pos_, start_drag_); break ;
    case RX:
         ray_plane_intersection(r, axis_x, start_pos_, start_drag_); break ;
    case RY:
        ray_plane_intersection(r, axis_y, start_pos_, start_drag_); break ;
    case RZ:
        ray_plane_intersection(r, axis_z, start_pos_, start_drag_); break ;

    }

    if ( cb_) cb_(TRANSFORM_GIZMO_MOTION_STARTED, transform_node_->transform()) ;

    dragging_ = c ;
    return true ;
}

bool TransformGizmo::onMouseReleased(QMouseEvent *) {
    for( int i=0 ; i<N_COMPONENTS ; i++ ) {
        if ( i != dragging_ ) {
            components_[i].node_->setVisible(true) ;
        }
    }
    dragging_ = -1 ;
    if ( cb_) cb_(TRANSFORM_GIZMO_MOTION_ENDED, transform_node_->transform()) ;

    return false ;
}

bool TransformGizmo::onMouseMoved(QMouseEvent *event) {
    if ( dragging_ != -1 ) {
        Ray r = camera_->getRay(event->x(), event->y()) ;

        Vector3f axis_x = ( local_ ) ? orientation_ * AXIS_X : AXIS_X;
        Vector3f axis_y = ( local_ ) ? orientation_ * AXIS_Y : AXIS_Y ;
        Vector3f axis_z = ( local_ ) ? orientation_ * AXIS_Z : AXIS_Z ;

        Vector3f pt ;
        switch (dragging_) {
        case TX:
            ray_plane_intersection(r, axis_x, start_pos_, camera_->eye(), pt);
            project_on_axis(pt, start_pos_, axis_x) ;
            setTranslation(pt - start_drag_) ;
           break ;
        case TY:
            ray_plane_intersection(r, axis_y, start_pos_, camera_->eye(), pt);
            project_on_axis(pt, start_pos_, axis_y) ;
            setTranslation(pt - start_drag_) ;
            break ;
        case TZ:
            ray_plane_intersection(r, axis_z, start_pos_, camera_->eye(), pt);
            project_on_axis(pt, start_pos_, axis_z) ;
            setTranslation(pt - start_drag_) ;
            break ;
        case TYZ:
            ray_plane_intersection(r, axis_x, start_pos_, pt);
            setTranslation(pt - start_drag_) ;
            break ;
        case TXZ:
            ray_plane_intersection(r, axis_y, start_pos_, pt);
            setTranslation(pt - start_drag_) ;
            break ;
        case TXY:
            ray_plane_intersection(r, axis_z, start_pos_, pt);
            setTranslation(pt - start_drag_) ;
            break ;
        case RX: {
            ray_plane_intersection(r, axis_x, start_pos_, pt);
            float angle = rotation_angle(pt, start_drag_, start_pos_, axis_x) ;
            setRotation(AngleAxisf(angle, AXIS_X).matrix()) ;
            break ;
        }
        case RY: {
            ray_plane_intersection(r, axis_y, start_pos_, pt);
            float angle = rotation_angle(pt, start_drag_, start_pos_, axis_y) ;
            setRotation(AngleAxisf(angle, AXIS_Y).matrix()) ;
            break ;
        }
        case RZ: {
            ray_plane_intersection(r, axis_z, start_pos_, pt);
            float angle = rotation_angle(pt, start_drag_, start_pos_, axis_z) ;
            setRotation(AngleAxisf(angle, AXIS_Z).matrix()) ;
            break ;
        }

        }

        if ( cb_) cb_(TRANSFORM_GIZMO_MOVING, transform_node_->transform()) ;

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


void TransformGizmo::attachTo(Node *node) {
    transform_node_ = node ;
    position_ = node->transform().translation() ;
    orientation_ =  node->transform().rotation() ;
    updateTransforms() ;
}

void TransformGizmo::setLocalTransform(bool v) {
    local_ = v ;
    attachTo(transform_node_);
}



int TransformGizmo::hitTest(QMouseEvent *event, RayCastResult &res)
{
    Ray ray = camera_->getRay(event->x(), event->y()) ;

    float tmin = std::numeric_limits<float>::max() ;
    int c = -1 ;
    for( size_t i=0 ; i<N_COMPONENTS ; i++ ) {
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
