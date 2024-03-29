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
    URDFLoader(URDFPackageResolver package_map, const std::string &name_prefix):
        package_map_(package_map), prefix_(name_prefix) {}

    URDFRobot parseFile(const std::string &urdf_file) ;
    URDFRobot parseString(const std::string &urdf_str) ;

private:
    void parseRobot(const pugi::xml_node &node, URDFRobot &rb, const std::string &path) ;
    void parseLink(const pugi::xml_node &node, URDFRobot &rb, const std::string &path) ;
    void parseJoint(const pugi::xml_node &node, URDFRobot &rb) ;
    bool buildTree(URDFRobot &rb);

    Eigen::Isometry3f parseOrigin(const pugi::xml_node &node) ;
    URDFGeometry *parseGeometry(const pugi::xml_node &node,  Eigen::Vector3f &sc, const std::string &path) ;
    std::string parseMaterial(const pugi::xml_node &node, URDFRobot &rb, const std::string &path) ;
    std::string resolveUri(const std::string &uri, const std::string &path);
    Eigen::Matrix3f parseInertia(const pugi::xml_node &node);
    Eigen::Isometry3f parsePose(const pugi::xml_node &node);

    URDFPackageResolver package_map_ ;

    bool parse_collision_geometry_ ;
    std::string prefix_ ;

};

} // namespace xviz

#endif
