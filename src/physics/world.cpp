#include <clsim/physics/world.hpp>
#include <clsim/physics/cloth.hpp>
#include <clsim/physics/solver.hpp>
#include <clsim/physics/collision_object.hpp>

#include <clsim/scene/geometry.hpp>

namespace clsim {

World::World()
{

}

World::~World() {
}

void World::setCloth(clsim::Cloth *cloth) {
    cloth_.reset(cloth) ;

    MaterialPtr material(new PhongMaterial({0, 1, 1, 1}));
    material->setSide(Material::Side::Both) ;

    NodePtr node(new Node) ;
    GeometryPtr mesh(new Geometry(Geometry::Triangles));
    node->addDrawable(mesh, material) ;

    cloth_visual_ = node ;
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
