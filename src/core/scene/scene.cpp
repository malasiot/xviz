#include <xviz/scene/scene.hpp>
#include <xviz/scene/node.hpp>
#include <xviz/scene/geometry.hpp>

#include <unordered_set>

#include "scene.pb.h"

using namespace Eigen ;
using namespace std ;

namespace xviz {

Vector3f Scene::geomCenter() const {
    Vector3f center {0, 0, 0} ;
    uint count = 0 ;

    visitNodes([&](const Node &node){
        Affine3f tf = node.globalTransform() ;
        for ( const auto &d: node.drawables() ) {
            GeometryPtr mesh = d.geometry() ;

            for( const Vector3f &v: mesh->vertices() ) {
                Vector3f p = tf * v ;
                center += p ;
                ++count ;
            }
        }
    });

    if ( count != 0 ) center /= count ;

    return center ;
}

float Scene::geomRadius(const Vector3f &center) const {
    float max_dist = 0.0 ;

    visitNodes([&](const Node &node){
        Affine3f tf = node.globalTransform() ;
        for ( const auto &d: node.drawables() ) {
            GeometryPtr mesh = d.geometry() ;

            for( const Vector3f &v: mesh->vertices() ) {
                Vector3f p = tf * v ;
                float dist = (p - center).squaredNorm() ;
                max_dist = std::max(max_dist, dist) ;
            }
        }
    });

    return sqrt(max_dist) ;
}

static void get_materials(const Scene *scene, unordered_set<Material *> &material_map) {
    scene->visitNodes([&](const Node &n) {
        for( const auto &dr: n.drawables() ) {
            MaterialPtr material = dr.material() ;
            material_map.emplace(material.get()) ;
        }
    });
}

const std::vector<Material *> Scene::materials() const {
    unordered_set<Material *> material_map ;

    visitNodes([&](const Node &n) {
        for( const auto &dr: n.drawables() ) {
            MaterialPtr material = dr.material() ;
            material_map.emplace(material.get()) ;
        }
    });

    return {material_map.begin(), material_map.end()} ;
}

const std::vector<Geometry *> Scene::geometries() const {
    unordered_set<Geometry *> geometry_map ;

    visitNodes([&](const Node &n) {
        for( const auto &dr: n.drawables() ) {
            GeometryPtr geom = dr.geometry() ;
            geometry_map.emplace(geom.get()) ;
        }
    });


    return { geometry_map.begin(), geometry_map.end() } ;
}

msg::Scene *Scene::write(const Scene &scene) {
    msg::Scene *msg_scene = new msg::Scene ;

    // write meshes

    uint64_t id = 0 ;
    std::map<const Geometry *, uint64_t> mesh_map ;
    for( const Geometry *geom: scene.geometries() ) {
        msg::Mesh *msg_mesh = msg_scene->add_meshes() ;

        mesh_map[geom] = id ;
        msg_mesh->set_id(id++) ;
        switch ( geom->ptype() ) {
        case Geometry::Lines:
            msg_mesh->set_type(msg::Mesh_PrimitiveType_LINES) ;
            break ;
        case Geometry::Points:
            msg_mesh->set_type(msg::Mesh_PrimitiveType_POINTS) ;
            break ;
        case Geometry::Triangles:
            msg_mesh->set_type(msg::Mesh_PrimitiveType_TRIANGLES) ;
            break ;
        }
        const auto &indices = geom->indices() ;
        for( uint i: indices )
            msg_mesh->add_indices(i) ;
        if ( !geom->vertices().empty() ) {
            msg::VertexAttributeBuffer *buffer = msg_mesh->add_attributes() ;
            buffer->set_type(msg::VertexAttributeBuffer_Type_VERTICES) ;
            for( const auto &v: geom->vertices() ) {
                buffer->add_data(v.x()) ;
                buffer->add_data(v.y()) ;
                buffer->add_data(v.z()) ;
            }
        }
        if ( !geom->normals().empty() ) {
            msg::VertexAttributeBuffer *buffer = msg_mesh->add_attributes() ;
            buffer->set_type(msg::VertexAttributeBuffer_Type_NORMALS) ;
            for( const auto &v: geom->normals() ) {
                buffer->add_data(v.x()) ;
                buffer->add_data(v.y()) ;
                buffer->add_data(v.z()) ;
            }
        }
        if ( !geom->colors().empty() ) {
            msg::VertexAttributeBuffer *buffer = msg_mesh->add_attributes() ;
            buffer->set_type(msg::VertexAttributeBuffer_Type_COLORS) ;
            for( const auto &v: geom->colors() ) {
                buffer->add_data(v.x()) ;
                buffer->add_data(v.y()) ;
                buffer->add_data(v.z()) ;
            }
        }

        for ( uint i=0 ; i<MAX_TEXTURES ; i++ ) {
            if ( !geom->texCoords(i).empty() ) {
                msg::VertexAttributeBuffer *buffer = msg_mesh->add_attributes() ;
                buffer->set_type(msg::VertexAttributeBuffer_Type_UV) ;
                for( const auto &v: geom->texCoords(i) ) {
                    buffer->add_data(v.x()) ;
                    buffer->add_data(v.y()) ;
                }
            }
        }
    }

    // write materials

    id = 0 ;
    std::map<const Material *, uint64_t> material_map ;
    for( const Material *material: scene.materials() ) {
        msg::Material *material_msg = msg_scene->add_material() ;
        material_msg->set_id(id) ;
        material_map[material] = id++ ;
        if ( const PhongMaterial *mat = dynamic_cast<const PhongMaterial *>(material)) {
            msg::PhongMaterial *msg_phong = new msg::PhongMaterial ;
            msg_phong->add_ambient(mat->ambientColor().x()) ;
            msg_phong->add_ambient(mat->ambientColor().y()) ;
            msg_phong->add_ambient(mat->ambientColor().z()) ;
            msg_phong->add_ambient(mat->ambientColor().w()) ;

            msg_phong->add_diffuse(mat->diffuseColor().x()) ;
            msg_phong->add_diffuse(mat->diffuseColor().y()) ;
            msg_phong->add_diffuse(mat->diffuseColor().z()) ;
            msg_phong->add_diffuse(mat->diffuseColor().w()) ;

            msg_phong->add_specular(mat->specularColor().x()) ;
            msg_phong->add_specular(mat->specularColor().y()) ;
            msg_phong->add_specular(mat->specularColor().z()) ;
            msg_phong->add_specular(mat->specularColor().w()) ;

            msg_phong->set_shininess(mat->shininess());

            if ( mat->diffuseTexture() ) {
                msg_phong->set_allocated_diffuse_texture(Texture2D::write(*mat->diffuseTexture())) ;
            }

            if ( mat->specularTexture() ) {
                msg_phong->set_allocated_specular_texture(Texture2D::write(*mat->specularTexture())) ;
            }

            material_msg->set_allocated_phong_material(msg_phong);
        }
    }

    // write nodes

    id = 0 ;

    std::map<NodePtr, msg::Node *> node_map ;
    for( const NodePtr &node: scene.nodes() ) {
        msg::Node *node_msg = msg_scene->add_nodes() ;
        node_msg->set_name(node->name()) ;
        node_msg->set_id(id++) ;
        node_map[node] = node_msg ;

        auto mat = node->matrix().matrix() ;

        for ( uint i=0 ; i<4 ; i++ )
            for( uint j=0 ; j<4 ; j++ )
                node_msg->add_mat(mat(i, j)) ;

        for ( const Drawable &d: node->drawables() ) {
            msg::Drawable *msg_dr = node_msg->add_drawables() ;
            msg_dr->set_mesh_id(mesh_map[d.geometry().get()]);
            msg_dr->set_material_id(material_map[d.material().get()]) ;
        }
    }

    // create hierarchy

    for( const NodePtr &node: scene.nodes() ) {
        msg::Node *node_msg = node_map[node] ;

        for( const NodePtr &child: node->children() ) {
            msg::Node *msg_child = node_map[child] ;
            node_msg->add_children(msg_child->id()) ;
        }
    }

    return msg_scene ;
}

static Vector4f parseVector4(const ::google::protobuf::RepeatedField< float >& buffer) {
    return { buffer.Get(0), buffer.Get(1), buffer.Get(2), buffer.Get(3)} ;
}

Scene *Scene::read(const msg::Scene &msg) {

    Scene *scene = new Scene ;

    // read meshes

    map<uint64_t, GeometryPtr> mesh_map ;

    for ( const msg::Mesh &mesh_msg: msg.meshes() ) {
        Geometry::PrimitiveType ptype ;
        switch ( mesh_msg.type() ) {
        case msg::Mesh_PrimitiveType_LINES:
            ptype = Geometry::Lines ;
            break ;
        case msg::Mesh_PrimitiveType_POINTS:
            ptype = Geometry::Points ;
            break ;
        case msg::Mesh_PrimitiveType_TRIANGLES:
            ptype = Geometry::Triangles ;
            break ;
        }
        GeometryPtr mesh(new Geometry(ptype)) ;

        mesh_map[mesh_msg.id()] = mesh ;
//        scene->addGeometry(GeometryPtr(new MeshGeometry(Mesh))) ;

        for ( const msg::VertexAttributeBuffer &msg_buffer: mesh_msg.attributes()) {
            uint tidx = 0 ;
            if ( msg_buffer.type() == msg::VertexAttributeBuffer_Type_VERTICES ) {
                for( uint i=0 ; i<msg_buffer.data_size() ; i+=3 ) {
                    mesh->vertices().emplace_back(msg_buffer.data(i), msg_buffer.data(i+1), msg_buffer.data(i+2));
                }
            } else if ( msg_buffer.type() == msg::VertexAttributeBuffer_Type_NORMALS ) {
                for( uint i=0 ; i<msg_buffer.data_size() ; i+=3 ) {
                    mesh->normals().emplace_back(msg_buffer.data(i), msg_buffer.data(i+1), msg_buffer.data(i+2));
                }
            } else if ( msg_buffer.type() == msg::VertexAttributeBuffer_Type_COLORS ) {
                for( uint i=0 ; i<msg_buffer.data_size() ; i+=3 ) {
                    mesh->colors().emplace_back(msg_buffer.data(i), msg_buffer.data(i+1), msg_buffer.data(i+2));
                }
            } else if ( msg_buffer.type() == msg::VertexAttributeBuffer_Type_UV && tidx < MAX_TEXTURES ) {
                for( uint i=0 ; i<msg_buffer.data_size() ; i+=2 ) {
                    mesh->texCoords(tidx).emplace_back(msg_buffer.data(i), msg_buffer.data(i+1));
                }
                ++tidx ;
            }
        }

        for( uint32_t idx: mesh_msg.indices() ) {
            mesh->indices().push_back(idx) ;
        }
    }

    // read materials

    std::map<uint64_t, MaterialPtr> material_map ;
    for( const msg::Material &material: msg.material() ) {
        if ( material.has_phong_material() ) {
            PhongMaterial *m = new PhongMaterial ;
            MaterialPtr mat(m) ;
            material_map[material.id()] = mat ;
    //        scene->addMaterial(mat) ;
            const msg::PhongMaterial &phong_msg = material.phong_material();
            m->setAmbientColor(parseVector4(phong_msg.ambient())) ;
            m->setDiffuseColor(parseVector4(phong_msg.diffuse())) ;
            m->setSpecularColor(parseVector4(phong_msg.specular())) ;
            m->setShininess(phong_msg.shininess());
            if ( phong_msg.has_diffuse_texture() )
                m->setDiffuseTexture(Texture2D::read(phong_msg.diffuse_texture())) ;
            if ( phong_msg.has_specular_texture() )
                m->setSpecularTexture(Texture2D::read(phong_msg.specular_texture())) ;
        }
    }

    // read nodes

    std::map<uint64_t, NodePtr> node_map ;
    for( const msg::Node &node_msg: msg.nodes() ) {
        NodePtr node(new Node) ;
        node_map[node_msg.id()] = node ;
        scene->addNode(node) ;

        node->setName(node_msg.name()) ;

        Matrix4f mat ;
        int count = 0 ;
        for( uint i=0 ; i<4 ; i++ )
            for ( uint j=0 ; j<4 ; j++ )
                mat(i, j) = node_msg.mat(count++) ;

        node->setTransform(Affine3f(mat)) ;

        for( const msg::Drawable &msg_dr: node_msg.drawables() ) {
            GeometryPtr mesh = mesh_map[msg_dr.mesh_id()] ;
            MaterialPtr material = material_map[msg_dr.material_id()] ;
            node->addDrawable(mesh, material) ;
        }
    }

    // make hierarchy

     for( const msg::Node &node_msg: msg.nodes() ) {
         NodePtr node = node_map[node_msg.id()] ;
         for ( uint child_id: node_msg.children() ) {
             NodePtr child = node_map[child_id] ;
             node->addChild(child) ;
         }
     }

     return scene ;
}

string SceneMessage::encode() const {
    msg::SceneMessage sc_msg ;
    if ( type_ == SceneMessageType::Data ) {
        sc_msg.set_allocated_scene(Scene::write(*scene_));
    } else if ( type_ == SceneMessageType::Url ) {
        sc_msg.set_url(url_) ;
    } else {

    }

    return sc_msg.SerializeAsString() ;
}

SceneMessage *SceneMessage::decode(const string &msg) {
    msg::SceneMessage sc_msg ;

    if ( !sc_msg.ParseFromString(msg) ) return nullptr ;

    if ( sc_msg.has_scene() ) {
        return new SceneMessage(Scene::read(sc_msg.scene())) ;
    }
    else if ( sc_msg.has_url() ) {
       return new SceneMessage(sc_msg.url()) ;
    }
    else return new SceneMessage() ;
}


}
