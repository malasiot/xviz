#include "bvh.hpp"

#include <Eigen/Geometry>

using namespace Eigen ;
using namespace std ;

void BVHFile::parse(const std::string &file_path) {
    ifstream strm(file_path) ;

    while ( !strm.eof() ) {
         string token ;
        strm >> token ;
        if ( token == "HIERARCHY" ) {
            strm >> token ;
            if ( token == "ROOT" )
                parseJoint(strm, nullptr) ;
            else
                throw runtime_error("Expected root joint") ;
        }
        else if ( token == "MOTION" ) parseMotionData(strm) ;
        else return ;
    }
}

void BVHFile::getJointLocalTransforms(uint frame, std::map<string, Isometry3f> &j)
{
 //   for( const auto &joint: joints_ ) {
 //       j.emplace(joint->name_, joint->local_[frame]) ;
 //   }
   getJointsLocalTransformRecursive(joints_[0].get(), frame, j) ;
}

void BVHFile::getJointsLocalTransformRecursive(BVHJoint *joint, uint frame, std::map<std::string,Eigen::Isometry3f> &j)
{
    Matrix4f m ;
    if ( joint->parent_ != nullptr )
        m = joint->parent_->lt_[frame].inverse() * joint->lt_[frame]  ;
    else
        m = joint->lt_[frame] ;

    j.emplace(joint->name_, m) ;

    for( auto child: joint->children_ ) {
        getJointsLocalTransformRecursive(child, frame, j) ;
    }

}

void BVHFile::getJointsTransformRecursive(const Matrix4f &ptr, BVHJoint *joint, uint frame, std::map<std::string,Eigen::Isometry3f> &j)
{
    Matrix4f m ;
    if ( joint->parent_ != nullptr )
        m = ptr * joint->lt_[frame]  ;
    else
        m = joint->lt_[frame] ;

    j.emplace(joint->name_, m) ;

    for( auto child: joint->children_ ) {
        getJointsTransformRecursive(m, child, frame, j) ;
    }

}

void BVHFile::getJointTransforms(uint frame, std::map<std::string, Eigen::Isometry3f> &j) {
    getJointsTransformRecursive(Eigen::Matrix4f::Identity(), joints_[0].get(), frame, j) ;
}

void BVHFile::drawJoints(const string &vfile, int frame)
{
    ofstream strm(vfile) ;

    map<string, Isometry3f> tr ;
    getJointTransforms(frame, tr) ;

    int c = 1 ;
    vector<Vector3f> vertices ;
    vector<int> indices ;
    for( const auto &j: joints_ ) {
        BVHJoint *parent = j->parent_ ;
        if ( parent != nullptr && parent->name_ != root_joint_ ) {
            Vector4f j0 = tr[parent->name_] * Vector4f(0, 0, 0, 1) ;
            Vector4f j1 = tr[j->name_] * Vector4f(0, 0, 0, 1) ;
            vertices.push_back(j0.head<3>()) ;
            vertices.push_back(j1.head<3>()) ;
            indices.push_back(c++) ;
            indices.push_back(c++) ;

            if ( j->children_.empty() ) {
                Vector3f es = j->end_site_ ;
                Vector4f je = tr[j->name_] * Vector4f(es.x(), es.y(), es.z(), 1) ;
                vertices.push_back(je.head<3>()) ;
                indices.push_back(c-1) ;
                indices.push_back(c++) ;
            }
        }
    }

    for( const auto &v: vertices )
        strm << "v " << v.cast<double>().adjoint() << endl ;

    for( int i = 0 ; i<indices.size() ; i+=2 )
        strm << "l " << indices[i] << ' ' << indices[i+1] << endl ;
}

void BVHFile::parseJoint(ifstream &strm, BVHJoint *parent) {
    string token ;
    float ox, oy, oz ;

    strm >> token  ;

    if ( parent == nullptr ) root_joint_ = token ;

    joints_.emplace_back(new BVHJoint) ;
    BVHJoint *joint = joints_.back().get() ;

    joint->name_ = token ;
    joint->parent_ = parent ;

    strm >> token ;
    if ( token != "{" ) throw runtime_error("Expected {") ;

    do {
        strm >> token ;
        if ( token == "OFFSET" ) {
            strm >> ox >> oy >> oz ;
            joint->offset_ = {ox, oy, oz} ;
        } else if ( token == "CHANNELS" ) {
            int nc ;
            strm >> nc ;
            for(int i=0 ; i<nc ; i++ ) {
                strm >> token ;
                if ( token == "Xposition" )
                    joint->channels_.push_back(BVHJoint::Channel::XPOSITION) ;
                else if ( token == "Yposition" )
                    joint->channels_.push_back(BVHJoint::Channel::YPOSITION) ;
                else if ( token == "Zposition" )
                    joint->channels_.push_back(BVHJoint::Channel::ZPOSITION) ;
                else if ( token == "Xrotation" )
                    joint->channels_.push_back(BVHJoint::Channel::XROTATION) ;
                else if ( token == "Yrotation" )
                    joint->channels_.push_back(BVHJoint::Channel::YROTATION) ;
                else if ( token == "Zrotation" )
                    joint->channels_.push_back(BVHJoint::Channel::ZROTATION) ;
            }
        } else if ( token == "End" ) {
            strm >> token ; //Site
            strm >> token ; // {
            strm >> token ; // OFFSET
            strm >> ox >> oy >> oz ;
            joint->end_site_ = { ox, oy, oz } ;
            strm >> token ;
            token.clear() ;
        } else if ( token == "JOINT" ) {
            parseJoint(strm, joint) ;
        }

    } while ( token != "}") ;

    if ( parent != nullptr ) {
        parent->children_.push_back(joint) ;
    }



}

void BVHFile::parseMotionData(ifstream &strm) {
    string token, token2 ;
    float val ;

    strm >> token ;

    if ( token == "Frames:" )
        strm >> num_frames_ ;
    else
        throw runtime_error("Expected Frames:") ;

    strm >> token >> token2 ;

    if ( token == "Frame" && token2 == "Time:")
        strm >> frame_time_ ;
    else
        throw runtime_error("Expected Frame Time:") ;

    for(uint i=0 ; i<num_frames_ ; i++ ) {
        for (auto &joint : joints_ ) {
            std::vector <float> data;
            for (uint j = 0; j < joint->channels_.size(); j++) {
                strm >> val ;
                data.emplace_back(val) ;
            }
            joint->setChannelData(data) ;
        }

    }
}


void BVHJoint::setChannelData(const std::vector<float> &data)
{
    assert(data.size() == channels_.size()) ;

    Matrix4f m = Matrix4f::Identity() ;
    Matrix4f o = Matrix4f::Identity() ;
    o.block<3, 1>(0, 3) = offset_ ;

    Matrix3f rX = Matrix3f::Identity() ;
    Matrix3f rY = Matrix3f::Identity() ;
    Matrix3f rZ = Matrix3f::Identity() ;

    for( uint c =0 ; c<channels_.size() ; c++  ) {
        Channel channel = channels_[c] ;
        Matrix4f v = Matrix4f::Identity();
        float val = data[c] ;
        if ( channel == Channel::XROTATION )
            v.block<3, 3>(0, 0) = AngleAxisf(val * M_PI/180.0, Vector3f::UnitX()).matrix() ;
        else if ( channel == Channel::YROTATION )
            v.block<3, 3>(0, 0) = AngleAxisf(val * M_PI/180.0, Vector3f::UnitY()).matrix() ;
        else if ( channel == Channel::ZROTATION )
            v.block<3, 3>(0, 0) = AngleAxisf(val * M_PI/180.0, Vector3f::UnitZ()).matrix() ;
        else if ( channel == Channel::XPOSITION )
            v(0, 3) = val ;
        else if ( channel == Channel::YPOSITION )
            v(1, 3) = val ;
        else if ( channel == Channel::ZPOSITION )
            v(2, 3) = val ;

        m =  m * v ;
    }

    local_.push_back(m) ;

    lt_.push_back(o * m) ;
}
