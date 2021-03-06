#ifndef XVIZ_URDF_ROBOT_HPP
#define XVIZ_URDF_ROBOT_HPP

#include <map>
#include <vector>
#include <Eigen/Geometry>
#include <memory>

namespace xviz {

struct URDFJoint {
    std::string parent_, child_, type_, mimic_joint_, name_ ;
    Eigen::Vector3f axis_ ;
    Eigen::Isometry3f origin_ = Eigen::Isometry3f::Identity() ;
    float upper_, lower_, mimic_offset_, mimic_multiplier_, effort_, velocity_ ;
    float damping_, friction_ ;
    float position_ = 0;

    Eigen::Isometry3f getMatrix() const ;
};

struct URDFMaterial ;

struct URDFGeometry {
    virtual ~URDFGeometry() {}
    std::string material_ref_ ;
    Eigen::Isometry3f origin_ = Eigen::Isometry3f::Identity() ;
} ;

struct URDFMeshGeometry: public URDFGeometry {
    std::string path_ ;
    Eigen::Vector3f scale_ ;
};

struct URDFBoxGeometry: public URDFGeometry {
    URDFBoxGeometry(const Eigen::Vector3f &he): he_(he) {}
    Eigen::Vector3f he_ ;
};

struct URDFSphereGeometry: public URDFGeometry {
    URDFSphereGeometry(float r): radius_(r) {}
    float radius_ ;
};

struct URDFCylinderGeometry: public URDFGeometry {
    URDFCylinderGeometry(float radius, float height): radius_(radius), height_(height) {}
    float radius_, height_ ;
};

struct URDFMaterial {
    std::string texture_path_ ;
    Eigen::Vector4f diffuse_color_ ;
};

struct URDFInertial {
   Eigen::Isometry3f origin_ = Eigen::Isometry3f::Identity() ;
   float mass_ ;
   Eigen::Matrix3f inertia_ ;
} ;

struct URDFLink {
    std::string name_ ;

    std::unique_ptr<URDFInertial> inertial_ ;
    std::unique_ptr<URDFGeometry> visual_geom_, collision_geom_ ;

    std::vector<URDFLink *> child_links_ ;
    std::vector<URDFJoint *> child_joints_ ;
    URDFLink *parent_link_ = nullptr ;
    URDFJoint *parent_joint_ = nullptr ;
};

struct URDFRobot {

    static URDFRobot load(const std::string &fname, const std::map<std::string, std::string> package_map, bool load_collision_geometry) ;

    URDFLink *getLink(const std::string &name)  ;
    URDFJoint *findJoint(const std::string &name);

    float setJointPosition(const std::string &jname, float pos) ;

    std::string name_ ;
    std::map<std::string, URDFJoint> joints_ ;
    std::map<std::string, URDFLink> links_ ;
    std::map<std::string, std::shared_ptr<URDFMaterial>> materials_ ;
    URDFLink *root_ ;

public:

    void computeLinkTransforms(std::map<std::string, Eigen::Isometry3f> &transforms) const ;
    void computeLinkTransformRecursive(std::map<std::string, Eigen::Isometry3f> &transforms,
                                       const URDFLink *link, const Eigen::Isometry3f &parent) const;
};

class URDFLoadException: public std::runtime_error {
public:
    URDFLoadException(const std::string &msg): std::runtime_error(msg) {}
};

} // namespace xviz


#endif
