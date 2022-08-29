#ifndef POSE_HPP
#define POSE_HPP

#include <Eigen/Geometry>
#include <map>
#include <vector>
#include <string>


class Pose {
public:

    Pose() {} ;

    void setBoneTransform(const std::string &name, const Eigen::Matrix4f &t) ;
    void setBoneTransform(const std::string &name, const Eigen::Quaternionf &t) ;

    bool getBoneTransform(const std::string &name, Eigen::Matrix4f &t) const ;
    bool getBoneTransform(const std::string &name, Eigen::Quaternionf &q) const ;

    void setGlobalTransform(const Eigen::Matrix4f &g) ;
    void setGlobalTransform(const Eigen::Quaternionf &q, const Eigen::Vector3f &t) ;

    void setGlobalScale(float scale) ;

    float getGlobalScale() const ;

    Eigen::VectorXd pack(const std::vector<std::string> &bones) const ;

    Eigen::Matrix4f getGlobalTransform() const ;

    static Pose unpack(const Eigen::VectorXd &data, const std::vector<std::string> &bones) ;

private:

    std::map<std::string, Eigen::Quaternionf> channels_ ;

    Eigen::Quaternionf global_rot_ = Eigen::Quaternionf::Identity()  ;
    Eigen::Vector3f global_trans_ = {0, 0, 0} ;
    float global_scale_ = 1.0 ;
};


#endif
