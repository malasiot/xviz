#include <xviz/robot/robot_scene.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/material.hpp>
#include <xviz/robot/urdf_robot.hpp>

#include <xviz/robot/urdf_loader.hpp>

using namespace std ;
using namespace Eigen ;

namespace xviz {

RobotScenePtr RobotScene::loadURDF(const string &filename, const map<string, string> &packages, bool load_collision_geometry) {

    URDFLoader loader(packages) ;
    URDFRobot rb = loader.parse(filename) ;
    return parseRobotURDF(rb, load_collision_geometry) ;
}

RobotScenePtr RobotScene::fromURDF(const URDFRobot &r, bool cg) {
    return parseRobotURDF(r, cg) ;
}

NodePtr createLinkGeometry(const URDFGeometry *urdf_geom, MaterialPtr mat, Vector3f &scale) {

    if ( const URDFMeshGeometry *mesh = dynamic_cast<const URDFMeshGeometry *>(urdf_geom) ) {
        NodePtr geom(new Node) ;

        geom->setName(mesh->path_) ;
        geom->load(mesh->path_, 0) ;

        // fill empty materials in loaded model with that provided in urdf ?
        if ( mat ) {
            geom->visit([&](Node &n) {
                for( auto &dr: n.drawables() ) {
                    if ( !dr.material() )
                        dr.setMaterial(mat) ;
                }
            }) ;
        }

        scale = mesh->scale_ ;

        return geom ;
    } else if ( const URDFBoxGeometry *box = dynamic_cast<const URDFBoxGeometry *>(urdf_geom) ) {
        NodePtr geom_node(new Node) ;

        GeometryPtr geom(new BoxGeometry(box->he_)) ;

        geom_node->addDrawable(geom, mat) ;

        return geom_node ;
    } else if ( const URDFCylinderGeometry *cylinder = dynamic_cast<const URDFCylinderGeometry *>(urdf_geom) ) {
        NodePtr geom_node(new Node) ;

        GeometryPtr geom(new CylinderGeometry(cylinder->radius_, cylinder->height_)) ;

        geom_node->addDrawable(geom, mat) ;

        return geom_node ;

    } else if ( const URDFSphereGeometry *sphere = dynamic_cast<const URDFSphereGeometry *>(urdf_geom) ) {
        NodePtr geom_node(new Node) ;

        GeometryPtr geom(new SphereGeometry(sphere->radius_)) ;

        geom_node->addDrawable(geom, mat) ;

        return geom_node ;
    }

    return nullptr ;

}

RobotScenePtr RobotScene::parseRobotURDF(const URDFRobot &rb, bool collision_geometry)
{
    RobotScenePtr scene(new RobotScene) ;

    map<string, MaterialPtr> materials ;

    // create materials

    for( const auto &mp: rb.materials_ ) {
        const URDFMaterial &mat = *mp.second ;
        const std::string &name = mp.first ;

        if ( mat.texture_path_.empty() ) {
            Vector4f clr = mat.diffuse_color_ ;
            PhongMaterial *material = new PhongMaterial ;
            material->setSide(Material::Side::Both) ;
            material->setShininess(0);
            material->setSpecularColor({0, 0, 0, 1}) ;
            material->setDiffuseColor(clr) ;

            materials.emplace(name, MaterialPtr(material)) ;
        } else {
            Image im(mat.texture_path_) ;
            Texture2D *s = new Texture2D(im, Sampler2D()) ;

            PhongMaterial *material = new PhongMaterial() ;
            material->setDiffuseTexture(s);
            material->setShininess(0);
            material->setSpecularColor({0, 0, 0, 1}) ;
            material->setDiffuseColor({0, 0, 0, 1}) ;

            materials.emplace(name, MaterialPtr(material)) ;
        }
    }

    // create links

    for( const auto &lp: rb.links_ ) {
        const URDFLink &link = lp.second ;


        if ( !collision_geometry ) {

            NodePtr link_node(new Node) ;
            link_node->setName(link.name_) ;

            Isometry3f local_inertial_frame = Isometry3f::Identity() ;
            if ( link.inertial_ )
                local_inertial_frame = link.inertial_->origin_ ;

            for( const auto &geom: link.visual_geoms_ ) {

                const std::string &matref = geom->material_ref_ ;

                MaterialPtr mat ;
                auto mat_it = materials.find(matref) ;
                if ( mat_it != materials.end() )
                    mat = mat_it->second ;

                Vector3f scale{1, 1, 1} ;
                NodePtr geom_node = createLinkGeometry(geom.get(), mat, scale) ;

                geom_node->setTransform(local_inertial_frame.inverse() * geom->origin_) ;

                link_node->addChild(geom_node) ;


            }

            scene->addChild(link_node) ;

        }


    }


    map<string, Isometry3f> trs ;
    rb.computeLinkTransforms(trs) ;
    scene->updateTransforms(trs) ;
    return scene ;

}

float RevoluteJoint::setPosition(float pos)
{
    Isometry3f tr ;
    tr.setIdentity() ;
    tr.rotate(AngleAxisf(pos, axis_)) ;
    node_->setTransform(tr) ;

    pos = std::max(pos, lower_limit_) ;
    pos = std::min(pos, upper_limit_) ;

    for( uint i=0 ; i<dependent_.size() ; i++ ) {
        float vpos = multipliers_[i] * pos + offsets_[i] ;
        if ( auto j = std::dynamic_pointer_cast<RevoluteJoint>(dependent_[i]) ) {
            j->setPosition(vpos) ;
        }
    }

    return pos ;
}

}
