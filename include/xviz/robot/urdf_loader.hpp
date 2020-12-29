#ifndef XVIZ_ROBOT_URDF_LOADER_HPP
#define XVIZ_ROBOT_URDF_LOADER_HPP

#include <map>
#include <pugi/pugixml.hpp>
#include <xviz/robot/urdf_robot.hpp>
#include <Eigen/Geometry>

namespace xviz {

class Geometry ;
class URDFRobot ;

class URDFLoader {
public:
    URDFLoader(const std::map<std::string, std::string> package_map, bool load_collision_geometry): package_map_(package_map),
        parse_collision_geometry_(load_collision_geometry) {}

    URDFRobot parse(const std::string &urdf_file) ;

private:
    void parseRobot(const pugi::xml_node &node, URDFRobot &rb, const std::string &path) ;
    void parseLink(const pugi::xml_node &node, URDFRobot &rb, const std::string &path) ;
    void parseJoint(const pugi::xml_node &node, URDFRobot &rb) ;
    bool buildTree(URDFRobot &rb);

    Eigen::Isometry3f parseOrigin(const pugi::xml_node &node) ;
    URDFGeometry *parseGeometry(const pugi::xml_node &node, const std::string &mat, Eigen::Vector3f &sc, const std::string &path) ;
    void parseMaterial(const pugi::xml_node &node, URDFRobot &rb, const std::string &path) ;
    std::string resolveUri(const std::string &uri, const std::string &path);
    Eigen::Matrix3f parseInertia(const pugi::xml_node &node);

    std::map<std::string, std::string> package_map_ ;

    bool parse_collision_geometry_ ;
};

} // namespace xviz

#endif
