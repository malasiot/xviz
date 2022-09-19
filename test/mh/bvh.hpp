#ifndef BVH_HPP
#define BVH_HPP

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <fstream>
#include <Eigen/Geometry>

struct BVHJoint {
public:
      enum class Channel {
        XPOSITION, YPOSITION, ZPOSITION, ZROTATION, XROTATION, YROTATION
      };

      std::string name_ ;
      BVHJoint *parent_ ;
      std::vector<BVHJoint *> children_ ;

      Eigen::Vector3f offset_, end_site_ ;
      std::vector <Channel> channels_;
      std::vector<std::vector <float>> channel_data_;
      std::vector <Eigen::Matrix4f> lt_, local_;
      std::vector <Eigen::Vector3f> pos_;

      ~BVHJoint() {}

      void setChannelData(const std::vector<float> &data) ;
};

class BVHFile {
public:
    void parse(const std::string &file_path) ;

    void getJointLocalTransforms(uint frame, std::map<std::string,Eigen::Isometry3f> &j) ;
    void getJointTransforms(uint frame, std::map<std::string,Eigen::Isometry3f> &j) ;

    BVHJoint *findJoint(const std::string &name) {
        for( const auto &j: joints_ )
            if ( j->name_ == name )
                return j.get() ;
        return nullptr ;
    }

    void drawJoints(const std::string &vfile, int frame) ;
    void printHierarchy() ;

private:

    void printHierarchy(BVHJoint *parent, size_t level);

    void parseJoint(std::ifstream &strm, BVHJoint *parent) ;
    void parseMotionData(std::ifstream &strm) ;
    void getJointsTransformRecursive(const Eigen::Matrix4f &parent, BVHJoint *current, uint frame, std::map<std::string,Eigen::Isometry3f> &j) ;
     void getJointsLocalTransformRecursive(BVHJoint *joint, uint frame, std::map<std::string, Eigen::Isometry3f> &j);
public:
    std::string root_joint_ ;
    std::vector<std::unique_ptr<BVHJoint>> joints_ ;
    int num_frames_ ;
    double frame_time_;
    unsigned num_channels_;

};



#endif
