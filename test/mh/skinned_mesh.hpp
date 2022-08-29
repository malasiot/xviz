#ifndef SKINNED_MESH_HPP
#define SKINNED_MESH_HPP

#include <human_tracker/pose.hpp>
#include <human_tracker/skeleton.hpp>
#include <human_tracker/mhx2.hpp>

#include <Eigen/Geometry>
#include <vector>

using PointList3f = std::vector<Eigen::Vector3f> ;

class SkinnedMesh {
public:

    using PointList3f = cvx::util::PointList3f ;


    struct Part {
        Bone *bone_ ;
        std::vector<uint> indices_ ;
    };


    SkinnedMesh() {}

    // apply the given bone transformations to the mesh and return new vertices and normals.

    void getTransformedVertices(const Pose &p, std::vector<Eigen::Vector3f> &mpos, std::vector<Eigen::Vector3f> &mnorm) const;

    void getTransformedVertices(const Pose &p, std::vector<Eigen::Vector3f> &mpos, std::vector<Eigen::Vector3f> &mnorm, Eigen::Vector3f& bboxMin, Eigen::Vector3f& bboxMax) const ;

    void getTransformedVertices(const Pose &p, const std::vector<uint32_t> indices, std::vector<Eigen::Vector3f> &mpos,Eigen::Vector3f& bboxMin, Eigen::Vector3f& bboxMax) const;

    void getTransformedVertices(const Pose &p, PointList3f &mpos) const ;

    void getTransformedVertices(const Pose &p, const std::vector<uint32_t> indices,
                                std::vector<Eigen::Vector3f> &mpos, std::vector<Eigen::Vector3f> &mnorm) const ;

    void getTransformedVertices(const Pose &p, const std::vector<uint32_t> indices, std::vector<Eigen::Vector3f> &mpos) const;


    void getTransformedVerticesPartial(const Pose &p, const std::vector<std::string> &bones, std::vector<Eigen::Vector3f> &mpos, std::vector<Eigen::Vector3f> &mnorm) const ;

    uint getDominantBone(const uint vtx_idx) const ;

    void computeWeights() ;

    Part makePart(const std::string &controlBone, const std::vector<std::string> &bones) const ;

public:
    struct VertexBoneData
    {
        std::vector<uint> id_ ;
        std::vector<float> weight_ ;

        void addBoneData(uint boneID, float w) ;
        void normalize() ;
    };

    // flat triangulated mesh geometry

    std::vector<Eigen::Vector3f> positions_ ;
    std::vector<Eigen::Vector3f> normals_ ;
    std::vector<uint> indices_ ;
    std::vector<VertexBoneData> bone_weights_ ;

    Skeleton skeleton_ ;

private:

    void makeColorMap(std::vector<Eigen::Vector3f> &clrs);
};

class MakeHumanSkinnedMesh: public SkinnedMesh {
public:
    MakeHumanSkinnedMesh(const MHX2Model &model, const std::string &meshName) ;

private:
    void createSkeleton(const MHX2Model &model);
    void createMesh(const MHX2Model &model, const std::string &meshName);
};

#endif
