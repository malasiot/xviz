#include <xviz/robot/urdf_loader.hpp>
#include <xviz/scene/drawable.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/material.hpp>

#include <set>

using namespace std ;
using namespace pugi ;

using namespace Eigen ;

namespace xviz {

URDFRobot URDFLoader::parse(const string &urdf_file) {
    URDFRobot robot ;

    xml_document doc ;

    xml_parse_result result = doc.load_file(urdf_file.c_str()) ;

    if ( !result )
        throw URDFLoadException(result.description()) ;

    xml_node root = doc.child("robot") ;

    if ( !root )
        throw URDFLoadException("No <robot> element found") ;

    robot.name_ = root.attribute("name").as_string() ;

    parseRobot(root, robot, urdf_file) ;

    buildTree(robot) ;

    return robot ;
}

void URDFLoader::parseRobot(const xml_node &node, URDFRobot &rb, const string &path) {

    for( const xml_node &n: node.children("material") )
        parseMaterial(n, rb, path) ;

    for( const xml_node &n: node.children("link") )
        parseLink(n, rb, path) ;

    for( const xml_node &n: node.children("joint") )
        parseJoint(n, rb) ;
}

void URDFLoader::parseLink(const xml_node &node, URDFRobot &rb, const string &path) {

    URDFLink link ;

    string name = node.attribute("name").as_string() ;

    if ( name.empty() )
        throw URDFLoadException("Attribute \"name\" missing from <link>") ;

    link.name_ = name ;

    if ( xml_node visual_node = node.child("visual") ) {
        Isometry3f tr ;
        tr.setIdentity() ;

        if ( xml_node origin_node = visual_node.child("origin") )
            tr = parseOrigin(origin_node) ;

        string matid ;

        if ( xml_node material_node = visual_node.child("material") ) {
            matid = material_node.attribute("name").as_string() ;
            parseMaterial(material_node, rb, path) ;
        }

        Vector3f scale{1, 1, 1} ;

        URDFGeometry *geom = nullptr ;

        if ( xml_node geom_node = visual_node.child("geometry") )
            geom = parseGeometry(geom_node, matid, scale, path) ;
        else
            throw URDFLoadException("<geometry> element is missing from <visual>") ;

        tr.linear() *= scale.asDiagonal() ;
        geom->origin_ = tr ;

        link.visual_geom_.reset(geom) ;
    }

    if ( xml_node collision_node = node.child("collision") ) {
        Isometry3f tr ;
        tr.setIdentity() ;

        if ( xml_node origin_node = collision_node.child("origin") )
            tr = parseOrigin(origin_node) ;

        Vector3f scale{1, 1, 1} ;

        URDFGeometry *geom ;

        if ( xml_node geom_node = collision_node.child("geometry") )
            geom = parseGeometry(geom_node, std::string(), scale, path) ;
        else
            throw URDFLoadException("<geometry> element is missing from <collision>") ;

        tr.linear() *= scale.asDiagonal() ;
        geom->origin_ = tr ;

        link.collision_geom_.reset(geom) ;
    }

    if ( xml_node inertial_node = node.child("inertial") ) {

        URDFInertial inr ;
        inr.origin_.setIdentity() ;
        inr.mass_ = 0.0;
        inr.inertia_.setIdentity() ;

        if ( xml_node origin_node = inertial_node.child("origin") )
            inr.origin_ = parseOrigin(origin_node) ;

        if ( xml_node mass_node = inertial_node.child("mass") )
            inr.mass_ = mass_node.attribute("value").as_float() ;

        if ( xml_node inertia_node = inertial_node.child("inertia") )
            inr.inertia_ = parseInertia(inertia_node) ;

        link.inertial_.reset(new URDFInertial(std::move(inr)));
    }



    rb.links_.insert(std::make_pair(name, std::move(link))) ;
}

static Vector3f parse_vec3(const std::string &s) {
    istringstream strm(s) ;
    float x, y, z ;
    strm >> x >> y >> z ;
    return {x, y, z} ;
}

static Vector4f parse_vec4(const std::string &s) {
    istringstream strm(s) ;
    float x, y, z, w ;
    strm >> x >> y >> z >> w;
    return {x, y, z, w} ;
}


void URDFLoader::parseJoint(const xml_node &node, URDFRobot &rb) {
    string name = node.attribute("name").as_string() ;
    string type = node.attribute("type").as_string() ;

    if ( name.empty() )
        throw URDFLoadException("<joint> is missing \"name\" attribute") ;

    URDFJoint j ;

    j.type_ = type ;
    j.name_ = name ;

    if ( xml_node origin_node = node.child("origin") )
        j.origin_ = parseOrigin(origin_node) ;

    if ( xml_node parent_node = node.child("parent") ) {
        string link_name = parent_node.attribute("link").as_string() ;
        j.parent_ = link_name ;
    }

    if ( xml_node child_node = node.child("child") ) {
        string link_name = child_node.attribute("link").as_string() ;
        j.child_ = link_name ;
    }

    if ( xml_node axis_node = node.child("axis") ) {
        string axis_str = axis_node.attribute("xyz").as_string() ;
        j.axis_ = parse_vec3(axis_str) ;
    }

    if ( xml_node limits_node = node.child("limit") ) {
        j.lower_ = limits_node.attribute("lower").as_float(0) ;
        j.upper_ = limits_node.attribute("upper").as_float(0) ;
        j.effort_ = limits_node.attribute("effort").as_float(0) ;
        j.velocity_ = limits_node.attribute("velocity").as_float(0) ;
    }

    if ( xml_node mimic_node = node.child("mimic") ) {
        j.mimic_joint_ = mimic_node.attribute("joint").as_string() ;
        j.mimic_offset_ = mimic_node.attribute("offset").as_float(0.0) ;
        j.mimic_multiplier_ = mimic_node.attribute("multiplier").as_float(1.0) ;
    }

    if ( xml_node dynamics_node = node.child("dynamics") ) {
        j.friction_ = dynamics_node.attribute("friction").as_float() ;
        j.damping_ = dynamics_node.attribute("damping").as_float() ;
    }

    rb.joints_.emplace(name, j) ;

}

bool URDFLoader::buildTree(URDFRobot &rb) {
    map<string, string> parent_link_tree ;

    for( auto &jp: rb.joints_ ) {
        URDFJoint &j = jp.second ;

        if ( j.child_.empty() || j.parent_.empty() ) return false ;

        URDFLink *child_link = rb.getLink(j.child_) ;
        URDFLink *parent_link = rb.getLink(j.parent_) ;

        if ( child_link == nullptr || parent_link == nullptr ) return false ;

        child_link->parent_link_ = parent_link ;
        child_link->parent_joint_ = &j ;
        parent_link->child_joints_.push_back(&j) ;
        parent_link->child_links_.push_back(child_link) ;
        parent_link_tree[child_link->name_] = j.parent_;
    }

    // find root

    rb.root_ = nullptr ;

    for ( const auto &lp: rb.links_ ) {
        const string &name = lp.first ;
        const URDFLink &l = lp.second ;
        auto it = parent_link_tree.find(name) ;
        if ( it == parent_link_tree.end() ) { // no parent thus root
            if ( rb.root_ == nullptr ) {
                URDFLink *l = rb.getLink(name) ;
                rb.root_ = l ;
            }
            else return false ;
        }
    }

    if ( rb.root_ == nullptr ) return false ;

    return true ;
}

Isometry3f URDFLoader::parseOrigin(const xml_node &node) {

    string xyz = node.attribute("xyz").as_string() ;
    string rpy = node.attribute("rpy").as_string() ;

    Isometry3f tr ;
    tr.setIdentity() ;

    if ( !xyz.empty() ) {
        Vector3f t = parse_vec3(xyz) ;
        tr.translate(t) ;
    }

    if ( !rpy.empty() ) {
        Vector3f r = parse_vec3(rpy) ;
        Quaternionf q ;

        q = AngleAxisf(r.z(), Vector3f::UnitZ()) * AngleAxisf(r.y(), Vector3f::UnitY()) * AngleAxisf(r.x(), Vector3f::UnitX());

        tr.rotate(q) ;
    }

    return tr ;
}

Matrix3f URDFLoader::parseInertia(const xml_node &node) {

    float ixx = node.attribute("ixx").as_float() ;
    float ixy = node.attribute("ixy").as_float() ;
    float ixz = node.attribute("ixz").as_float() ;
    float iyy = node.attribute("iyy").as_float() ;
    float iyz = node.attribute("iyz").as_float() ;
    float izz = node.attribute("izz").as_float() ;

    Matrix3f i ;
    i << ixx, ixy, ixz,
         ixy, iyy, iyz,
         ixz, iyz, izz ;

    return i ;
}

static std::string parentPath(const std::string &fpath) {
    size_t pos = fpath.find_last_of('/') ;
    return fpath.substr(0, pos) ;
}

string URDFLoader::resolveUri(const std::string &uri, const std::string &path) {

    string rpath ;

    if ( uri.substr(0, 10) == "package://" ) {
        size_t pos = uri.find_first_of('/', 10) ;
        if ( pos == string::npos ) return rpath ;
        string package_str = uri.substr(10, pos-10) ;
        string package_subpath = uri.substr(pos+1) ;

        auto it = package_map_.find(package_str) ;
        if ( it == package_map_.end() ) return rpath ;
        rpath = it->second + '/' + package_subpath ;
    } else {
        rpath = parentPath(path) + '/' + uri ;
    }

    return rpath;

}

URDFGeometry *URDFLoader::parseGeometry(const xml_node &node, const std::string &mat, Vector3f &sc, const string &path) {

    if ( xml_node mesh_node = node.child("mesh") ) {

        URDFMeshGeometry *geom = new URDFMeshGeometry() ;

        string uri = mesh_node.attribute("filename").as_string() ;

        string rpath = resolveUri(uri, path) ;

        if ( rpath.empty() ) return nullptr ;

        geom->path_ = rpath ;

        string scale = mesh_node.attribute("scale").as_string() ;

        if ( !scale.empty() ) {
            sc = parse_vec3(scale) ;
        }

        geom->scale_ = sc ;
        geom->material_ref_ = mat ;

        return geom ;
    } else if ( xml_node box_node = node.child("box") ) {
        string sz = box_node.attribute("size").as_string() ;
        Vector3f hs = parse_vec3(sz)/2 ;


        URDFBoxGeometry *geom = new URDFBoxGeometry(hs) ;
        geom->material_ref_ = mat ;

        return geom ;
    } else if ( xml_node cylinder_node = node.child("cylinder") ) {

        float radius = cylinder_node.attribute("radius").as_float(0) ;
        float length = cylinder_node.attribute("length").as_float(0) ;

        URDFCylinderGeometry *geom = new URDFCylinderGeometry(radius, length) ;

        geom->material_ref_ = mat ;

        return geom ;
    } else if ( xml_node cylinder_node = node.child("sphere") ) {

        float radius = cylinder_node.attribute("radius").as_float(0) ;

        URDFSphereGeometry *geom = new URDFSphereGeometry(radius) ;

        geom->material_ref_ = mat ;

        return geom ;
    }

    return nullptr ;
}

void URDFLoader::parseMaterial(const xml_node &node, URDFRobot &rb, const string &path)
{
    string name = node.attribute("name").as_string() ;
    if ( name.empty() ) return ;

    if ( xml_node clr_node = node.child("color") ) {
        string rgba = clr_node.attribute("rgba").as_string() ;
        if ( !rgba.empty() ) {
            Vector4f clr = parse_vec4(rgba) ;

            URDFMaterial *mat = new URDFMaterial ;
            mat->diffuse_color_ = clr ;
            rb.materials_.emplace(name, std::shared_ptr<URDFMaterial>(mat)) ;
            return ;
        }
    }

    if ( xml_node texture_node = node.child("texture") ) {
        string uri = texture_node.attribute("filename").as_string() ;

        string rpath = resolveUri(uri, path) ;
        if ( !rpath.empty() ) {

            URDFMaterial *mat = new URDFMaterial ;
            mat->texture_path_ = rpath ;

            rb.materials_.emplace(name, std::shared_ptr<URDFMaterial>(mat)) ;
            return ;
        }
    }
}

}


