#ifndef XVIZ_ROBOT_SCENE_HPP
#define XVIZ_ROBOT_SCENE_HPP

#include <xviz/scene/scene.hpp>
#include <xviz/robot/urdf_robot.hpp>

namespace xviz {

class URDFLoader ;

class JointNode ;
typedef std::shared_ptr<JointNode> JointNodePtr ;
// encapsulates degrees of freedom information

class JointNode {
public:

    enum Type { Revolute, Prismatic, Floating } ;

    JointNode(Type t): type_(t) {}

    virtual ~JointNode() {}

    Type type() const { return type_ ; }


    Type type_ ;
    NodePtr node_ ;
    std::vector<JointNodePtr> dependent_ ;
    std::vector<float> offsets_, multipliers_ ; // offsets and multipliers of dependent nodes
};

class RevoluteJoint: public JointNode {
public:
    RevoluteJoint(): JointNode(Revolute) {}

    float setPosition(float pos) ;

    float upper_limit_, lower_limit_ ;
    Eigen::Vector3f axis_ ;
};

class RobotScene ;
typedef std::shared_ptr<RobotScene> RobotScenePtr ;

// a scene with associated joint information
// Joints are represented by inserting nodes between links

class RobotScene: public Scene {
public:

    static RobotScenePtr loadURDFfromFile(const std::string &filename, const std::string &name_prefix = {}, URDFPackageResolver res = nullptr,  bool collision_geometries = false) ;

    static RobotScenePtr loadURDFfromString(const std::string &filename, const std::string &name_prefix = {}, URDFPackageResolver res = nullptr,  bool collision_geometries = false) ;

    static RobotScenePtr fromURDF(const URDFRobot &r, bool collision_geometries = false) ;

    JointNodePtr getJoint(const std::string &name) {
        auto it = joints_.find(name) ;
        if ( it != joints_.end() ) return it->second ;
        else return nullptr ;
    }

private:

    static RobotScenePtr parseRobotURDF(const URDFRobot &rb, bool collision_geometry = false) ;

    std::map<std::string, JointNodePtr> joints_ ;
};

} // namespace xviz

#endif
