#include <xviz/robot/urdf_robot.hpp>
#include <xviz/robot/urdf_loader.hpp>

#include <iostream>

using namespace std ;
using namespace Eigen ;

namespace xviz {

URDFRobot URDFRobot::loadFile(const std::string &filename, const string &name_prefix, URDFPackageResolver res) {
    URDFLoader loader(res, name_prefix) ;
    URDFRobot rb = loader.parseFile(filename) ;

    return rb ;
}

URDFRobot URDFRobot::loadString(const std::string &desc, const string &name_prefix, URDFPackageResolver res) {
    URDFLoader loader(res, name_prefix) ;
    URDFRobot rb = loader.parseString(desc) ;

    return rb ;
}

const URDFLink *URDFRobot::getLink(const std::string &name) const {
    auto it = links_.find(name) ;
    if ( it == links_.end() ) return nullptr ;
    else return &(it->second) ;
}

const URDFJoint *URDFRobot::findJoint(const std::string &name) const {
    auto it = joints_.find(name) ;
    if ( it == joints_.end() ) return nullptr ;
    else return &(it->second) ;
}

URDFLink *URDFRobot::getLink(const std::string &name) {
    auto it = links_.find(name) ;
    if ( it == links_.end() ) return nullptr ;
    else return &(it->second) ;
}

URDFJoint *URDFRobot::findJoint(const std::string &name) {
    auto it = joints_.find(name) ;
    if ( it == joints_.end() ) return nullptr ;
    else return &(it->second) ;
}

float URDFRobot::setJointPosition(const std::string &name, float pos)
{
    URDFJoint *j = findJoint(name) ;

    if ( j != nullptr && j->mimic_joint_.empty() ) {
        pos = std::max(pos, j->lower_) ;
        pos = std::min(pos, j->upper_) ;
        j->position_ = pos ;

        for( auto &jp: joints_ ) {
            URDFJoint &joint = jp.second ;
            if ( joint.mimic_joint_ == name ) {
                joint.position_ = pos * joint.mimic_multiplier_ + joint.mimic_offset_ ;
            }
        }
    }

    return pos ;
}

void URDFRobot::computeLinkTransforms(std::map<std::string, Isometry3f> &transforms) const
{
    Isometry3f parent ;
    parent.setIdentity() ;
    computeLinkTransformRecursive(transforms, root_, parent) ;
}

void URDFRobot::computeLinkTransformRecursive(std::map<std::string, Isometry3f> &transforms, const URDFLink *link, const Isometry3f &parent) const
{
    Isometry3f p2j, tr ;
    p2j.setIdentity() ;

    URDFJoint *parent_joint = link->parent_joint_ ;
    Isometry3f local_inertial_frame = Isometry3f::Identity() ;
    if ( link->inertial_ )
        local_inertial_frame = link->inertial_->origin_;

    if ( parent_joint ) {
        p2j = parent_joint->getMatrix() ;

    }

    tr = parent * p2j ;
    transforms.emplace(link->name_, global_ * tr) ;

    for( const URDFLink *l: link->child_links_ ) {
        computeLinkTransformRecursive(transforms, l, tr) ;
    }
}


Isometry3f URDFJoint::getMatrix() const {
    Isometry3f tr ;
    tr.setIdentity() ;

    if ( type_ == "revolute" || type_ == "continuous" ) {
        tr.rotate(AngleAxisf(position_, axis_)) ;
    } else if ( type_ == "prismatic" ) {
        tr.translate(axis_* position_) ;
    }

    return origin_ * tr ;
}


}

