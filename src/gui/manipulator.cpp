#include <xviz/gui/manipulator.hpp>
#include <xviz/scene/node_helpers.hpp>
#include "renderer/util.hpp"
#include <QMouseEvent>
#include <QDebug>

#include <iostream>
using namespace Eigen ;
using namespace std ;

namespace xviz {

TransformGizmo::TransformGizmo(float radius) {

    ray_caster_.setBackFaceCulling(false);

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

void TransformGizmo::setVisible(bool v, int c) {
    for( int i=0 ; i<N_COMPONENTS ; i++ ) {
        if ( i != c )
            components_[i].node_->setVisible(v) ;
    }
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


void TransformGizmo::Component::setMaterialColor(const Vector4f &clr)
{
    ConstantMaterial *cm = static_cast<ConstantMaterial *>(mat_.get()) ;
    cm->setColor(clr) ;
}

TransformManipulator::TransformManipulator(const CameraPtr &cam, float radius): Manipulator(), camera_(cam)
{
    gizmo_.reset(new TransformGizmo(radius)) ;
    gizmo_->setOrder(10) ;
}

void TransformManipulator::setTranslation(const Vector3f &v)
{
    position_ = start_pos_ + v ;
    updateTransforms() ;
}

void TransformManipulator::setRotation(const Matrix3f &m)
{
    if ( local_ )
        orientation_ = start_orientation_ * m ;
    else
        orientation_ = m ;

    updateTransforms() ;
}

void TransformManipulator::updateTransforms()
{
    Isometry3f tr = Isometry3f::Identity() ;

    tr.translate(position_) ;
    tr.rotate(orientation_) ;

    transform_node_->transform() = orig_ * tr ;

    if ( local_ ) {
        gizmo_->transform() = orig_ * tr ;
    } else {
        gizmo_->transform().translation() = orig_ * position_ ;
        gizmo_->transform().linear() = Matrix3f::Identity() ;
    }

}

Vector3f TransformManipulator::globalPosition() const
{
    return transform_node_->parent()->globalTransform() * position_ ;
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

void ray_ray_intersection(const Ray &ray, const Vector3f &p, const Vector3f &dir, Vector3f &intersection) {
    Vector3f da = ray.dir();
    Vector3f db = dir ;
    Vector3f dc = p - ray.origin() ;

    Vector3f cab = da.cross(db) ;

    float s = dc.cross(da).dot(cab) / cab.squaredNorm() ;
    intersection = p + s * dir ;
}

static Vector3f AXIS_X{1, 0, 0} ;
static Vector3f AXIS_Y{0, 1, 0} ;
static Vector3f AXIS_Z{0, 0, 1} ;
static Vector3f ZERO{0, 0, 0} ;

bool TransformManipulator::onMousePressed(QMouseEvent *event)
{
    RayCastResult rc ;
    int c = gizmo_->hitTest(event->x(), event->y(), camera_, rc) ;
    if ( c == -1 ) return false ;

    Ray r = camera_->getRay(event->x(), event->y()) ;
    start_tr_ = transform_node_->globalTransform().inverse();
    Ray tr(r, start_tr_) ;

    start_pos_ = position_ ;
    start_orientation_ = orientation_ ;

    gizmo_->setVisible(false, c) ;

    float t ;
    switch ( c ) {
    case TransformGizmo::TX:
        ray_ray_intersection(tr, ZERO, AXIS_X, start_drag_) ; break ;
    case TransformGizmo::TY:
        ray_ray_intersection(tr, ZERO, AXIS_Y, start_drag_) ; break ;
    case TransformGizmo::TZ:
        ray_ray_intersection(tr, ZERO, AXIS_Z, start_drag_) ; break ;
    case TransformGizmo::TYZ:
        ray_plane_intersection(tr, AXIS_X, ZERO, start_drag_); break ;
    case TransformGizmo::TXZ:
        ray_plane_intersection(tr, AXIS_Y, ZERO, start_drag_); break ;
    case TransformGizmo::TXY:
        ray_plane_intersection(tr, AXIS_Z, ZERO, start_drag_); break ;
    case TransformGizmo::RX:
         ray_plane_intersection(tr, AXIS_X, ZERO, start_drag_); break ;
    case TransformGizmo::RY:
        ray_plane_intersection(tr, AXIS_Y, ZERO, start_drag_); break ;
    case TransformGizmo::RZ:
        ray_plane_intersection(tr, AXIS_Z, ZERO, start_drag_); break ;

    }

    if ( cb_) cb_(TRANSFORM_MANIP_MOTION_STARTED, transform_node_->transform()) ;

    dragging_ = c ;
    return true ;
}

bool TransformManipulator::onMouseReleased(QMouseEvent *) {
    gizmo_->setVisible(true, dragging_) ;

    if ( cb_ && dragging_ != -1 ) {
        cb_(TRANSFORM_MANIP_MOTION_ENDED, transform_node_->transform()) ;
    }

    dragging_ = -1 ;
    return false ;
}

bool TransformManipulator::onMouseMoved(QMouseEvent *event) {
    if ( dragging_ != -1 ) {
        Ray r = camera_->getRay(event->x(), event->y()) ;
        Ray tr(r, start_tr_) ;

        Vector3f pt ;

        switch (dragging_) {
        case TransformGizmo::TX:
            ray_ray_intersection(tr, ZERO, AXIS_X, pt) ;
            setTranslation(pt - start_drag_) ;
            break ;
        case TransformGizmo::TY:
            ray_ray_intersection(tr, ZERO, AXIS_Y, pt) ;
            setTranslation(pt - start_drag_) ;
            break ;
        case TransformGizmo::TZ:
            ray_ray_intersection(tr, ZERO, AXIS_Z, pt) ;
            setTranslation(pt - start_drag_) ;
            break ;
        case TransformGizmo::TYZ:
            ray_plane_intersection(tr, AXIS_X, ZERO, pt);
            setTranslation(pt - start_drag_) ;
            break ;
        case TransformGizmo::TXZ:
            ray_plane_intersection(tr, AXIS_Y, ZERO, pt);
            setTranslation(pt - start_drag_) ;
            break ;
        case TransformGizmo::TXY:
            ray_plane_intersection(tr, AXIS_Z, ZERO, pt);
            setTranslation(pt - start_drag_) ;
            break ;
        case TransformGizmo::RX: {
            ray_plane_intersection(tr, AXIS_X, ZERO, pt);
            float angle = rotation_angle(pt, start_drag_, ZERO, AXIS_X) ;
            setRotation(AngleAxisf(angle, AXIS_X).matrix()) ;
            break ;
        }
        case TransformGizmo::RY: {
            ray_plane_intersection(tr, AXIS_Y, ZERO, pt);
            float angle = rotation_angle(pt, start_drag_, ZERO, AXIS_Y) ;
            setRotation(AngleAxisf(angle, AXIS_Y).matrix()) ;
            break ;
        }
        case TransformGizmo::RZ: {
            ray_plane_intersection(tr, AXIS_Z, ZERO, pt);
            float angle = rotation_angle(pt, start_drag_, ZERO, AXIS_Z) ;
            setRotation(AngleAxisf(angle, AXIS_Z).matrix()) ;
            break ;
        }

        }

        if ( cb_) cb_(TRANSFORM_MANIP_MOVING, transform_node_->transform()) ;

        return true ;
    } else {
        RayCastResult r ;
        int c = gizmo_->hitTest(event->x(), event->y(), camera_, r) ;
        gizmo_->setSelection(c) ;
        return ( c != -1 )  ;
    }

    return false ;
}



void TransformManipulator::attachTo(Node *node) {
    transform_node_ = node ;
    orig_ = node->transform() ;
    Node * parent = node->parent() ;
    assert(parent) ;
    // reparent gizmo to be a sibling of transform node
    if ( gizmo_->parent() )
        gizmo_->parent()->removeChild(gizmo_);
    parent->addChild(gizmo_) ;
    updateTransforms() ;
}

void TransformManipulator::setLocalTransform(bool v) {
    local_ = v ;
    updateTransforms();
}

int TransformGizmo::hitTest(int x, int y, const CameraPtr &cam, RayCastResult &res) {

    Ray ray = cam->getRay(x, y) ;

    float tmin = std::numeric_limits<float>::max() ;
    int c = -1 ;
    for( size_t i=0 ; i<N_COMPONENTS ; i++ ) {
        if ( !components_[i].node_ ) continue ;
        RayCastResult r ;
        if ( ray_caster_.intersectOne(ray, components_[i].node_, r) && r.t_ < tmin ) {
            tmin = r.t_ ;
            c = i ;
            res = r ;
        }
    }

    return c ;
}



}
