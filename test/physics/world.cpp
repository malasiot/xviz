#include "world.hpp"
#include "cloth.hpp"
#include "solver.hpp"
#include "collision_object.hpp"

#include <xviz/scene/geometry.hpp>

namespace xviz {

World::World()
{

}

World::~World() {
}

void World::setCloth(xviz::Cloth *cloth) {
    cloth_.reset(cloth) ;

    size_t np = cloth_->particles_.size() ;

    MaterialPtr material(new PhongMaterial({0, 1, 1, 1}));
    material->setSide(Material::Side::Both) ;

    NodePtr node(new Node) ;
    GeometryPtr mesh(new Geometry(Geometry::Triangles));
    node->addDrawable(mesh, material) ;

    cloth_visual_ = node ;

    collision_visual_.reset(new Node) ;

    MaterialPtr cm(new PerVertexColorMaterial()) ;
    GeometryPtr collision_geom(new Geometry(Geometry::Lines));
    collision_geom->vertices().resize(np * 2, {0, 0, 0}) ;
    collision_geom->colors().resize(np * 2, {1, 0, 0}) ;

    collision_visual_->addDrawable(collision_geom, cm) ;

}

void World::setSolver(Solver *solver)
{
    solver_.reset(solver) ;
}

void World::addCollisionObject(CollisionObject *obj) {
    std::unique_ptr<CollisionObject> co(obj);
    objects_.emplace_back(std::move(co)) ;
}

void World::resetSimulation()
{
    solver_->init() ;
    updateVisuals() ;
}

void World::stepSimulation(float dt) {
    solver_->step(dt) ;
    updateVisuals() ;
}

NodePtr World::getVisual() const
{
    NodePtr node(new Node) ;

    node->addChild(cloth_visual_) ;
    for( const auto &c: objects_ ) {
        node->addChild(c->getVisual()) ;
    }

    node->addChild(collision_visual_) ;

    return node ;
}

void World::updateVisuals()
{
    if ( cloth_visual_ ) {
        auto &dr = cloth_visual_->drawables()[0] ;

        GeometryPtr geom = dr.geometry() ;
        geom->vertices().clear() ;
        geom->normals().clear() ;
        cloth_->getMesh(geom->vertices(), geom->normals(), geom->indices());
        geom->computeNormals() ;
        geom->setVerticesUpdated(true);
        geom->setNormalsUpdated(true);
    }
}

}
