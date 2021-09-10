#include <clsim/physics/world.hpp>
#include <clsim/physics/cloth.hpp>
#include <clsim/physics/solver.hpp>

#include <clsim/scene/geometry.hpp>

namespace clsim {

void World::setCloth(clsim::Cloth *cloth) {
    cloth_.reset(cloth) ;

    MaterialPtr material(new PhongMaterial({0, 1, 1, 1}));
    material->setSide(Material::Side::Both) ;

    NodePtr node(new Node) ;
    GeometryPtr mesh(new Geometry(Geometry::Triangles));
    node->addDrawable(mesh, material) ;

    cloth_->getMesh(mesh->vertices(), mesh->normals(), mesh->indices());
    mesh->setVerticesUpdated(true);
    mesh->setNormalsUpdated(true);

    cloth_visual_ = node ;
}

void World::setSolver(Solver *solver)
{
    solver_.reset(solver) ;
}

void World::resetSimulation()
{
    solver_->init() ;
}

void World::stepSimulation(float dt) {
    solver_->step(dt) ;
    updateVisuals() ;
}

void World::updateVisuals()
{
    if ( cloth_visual_ ) {
        auto &dr = cloth_visual_->drawables()[0] ;

        GeometryPtr geom = dr.geometry() ;
        geom->vertices().clear() ;
        geom->normals().clear() ;
        cloth_->getMesh(geom->vertices(), geom->normals(), geom->indices());
        geom->setVerticesUpdated(true);
        geom->setNormalsUpdated(true);
    }
}

}
