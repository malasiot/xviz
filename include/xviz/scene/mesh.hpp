#ifndef XVIZ_SCENE_MESH_HPP
#define XVIZ_SCENE_MESH_HPP

#include <xviz/scene/scene_fwd.hpp>
#include <vector>

#include <Eigen/Geometry>

namespace xviz {

const int MAX_TEXTURES = 2 ;

class Mesh: public std::enable_shared_from_this<Mesh> {
public:

    enum PrimitiveType { Triangles, Lines, Points } ;

    using indices_t = std::vector<uint32_t> ;

    Mesh(PrimitiveType t): ptype_(t) {}
     ~Mesh();

    using vb3_t = std::vector<Eigen::Vector3f> ;
    using vb2_t = std::vector<Eigen::Vector2f> ;

    vb3_t &vertices() { return vertices_ ; }
    vb3_t &normals() { return normals_ ; }
    vb3_t &colors() { return colors_ ; }
    vb2_t &texCoords(uint t) {
        assert(t<MAX_TEXTURES) ;
        return tex_coords_[t] ;
    }

    indices_t &indices() { return indices_ ; }
    const indices_t &indices() const { return indices_ ; }

    const vb3_t &vertices() const { return vertices_ ; }
    const vb3_t &normals() const { return normals_ ; }
    const vb3_t &colors() const { return colors_ ; }
    const vb2_t &texCoords(uint t) const {
        assert(t<MAX_TEXTURES) ;
        return tex_coords_[t] ;
    }

    static const int MAX_BONES_PER_VERTEX = 4 ;

    struct BoneWeight {
        int bone_[MAX_BONES_PER_VERTEX] ;
        float weight_[MAX_BONES_PER_VERTEX] ;

        BoneWeight() {
            std::fill(bone_, bone_ + MAX_BONES_PER_VERTEX, -1) ;
            std::fill(weight_, weight_ + MAX_BONES_PER_VERTEX, 0.f) ;
        }

        void add(int bone, float w) {
            int idx = 0 ;
            while ( bone_[idx] != -1 && idx < MAX_BONES_PER_VERTEX ) ++idx ;
            bone_[idx] = bone ;
            weight_[idx] = w ;
        }

        void normalize() {
            float w = 0.0 ;

               for(int i=0 ; i<MAX_BONES_PER_VERTEX ; i++) {
                   if ( bone_[i] < 0 ) break ;
                   w += weight_[i] ;
               }

               if ( w == 0.0 ) return ;

               for(int i=0 ; i<MAX_BONES_PER_VERTEX ; i++) {
                   if ( bone_[i] < 0 ) break ;
                   weight_[i] /= w  ;
               }
        }
    };
    /*

    const std::vector<Bone> &skeleton() const { return skeleton_ ; }
    std::vector<Bone> &skeleton() { return skeleton_ ; }

    Eigen::Affine3f &skeletonInverseGlobalTransform() { return skeleton_inverse_global_transform_ ; }

    const std::vector<BoneWeight> &weights() const { return weights_ ; }
    std::vector<BoneWeight> &weights() { return weights_ ; }

    bool hasSkeleton() const { return !skeleton_.empty() ; }
*/
    PrimitiveType ptype() const { return ptype_ ; }

    // it is a simple triangle mesh with per-pertex attributes
    bool isSimpleIndexed() const ;

    // primitive shape factories
/*
    static MeshPtr createWireCube(const Eigen::Vector3f &hs) ;
    static MeshPtr createSolidCube(const Eigen::Vector3f &hs) ;

    static MeshPtr createWireSphere(float radius, size_t slices, size_t stacks) ;
    static MeshPtr createSolidSphere(float radius, size_t slices, size_t stacks) ;

    // the base of the cone is on (0, 0, 0) aligned with the z-axis and pointing towards positive z

    static MeshPtr createWireCone(float radius, float height, size_t slices, size_t stacks) ;
    static MeshPtr createSolidCone(float radius, float height, size_t slices, size_t stacks) ;

    static MeshPtr createWireCylinder(float radius, float height, size_t slices, size_t stacks) ;
    static MeshPtr createSolidCylinder(float radius, float height, size_t slices, size_t stacks) ;

    static MeshPtr createCapsule(float radius, float height, size_t slices, size_t head_stacks, size_t body_stacks) ;

    static MeshPtr makePointCloud(const cvx::util::PointList3f &pts) ;
    static MeshPtr makePointCloud(const cvx::util::PointList3f &coords, const cvx::util::PointList3f &clrs) ;

    void computeNormals() ;
    void computeBoundingBox(Eigen::Vector3f &bmin, Eigen::Vector3f &bmax) const ;
    void makeOctree() ;

//    bool intersect(const cvx::viz::Ray &ray, float &t) const override ;

    // create a new mesh without indices
    static MeshPtr flatten(const MeshPtr &src) ;
*/
    // get underlying OpenGL buffer data

private:

    vb3_t vertices_, normals_, colors_ ;
    vb2_t tex_coords_[MAX_TEXTURES] ;
    indices_t indices_ ;
    std::vector<BoneWeight> weights_ ;
//    std::vector<Bone> skeleton_ ;
//    Eigen::Affine3f skeleton_inverse_global_transform_ = Eigen::Affine3f::Identity() ;

    PrimitiveType ptype_ ;
//    detail::Octree *octree_ = nullptr ;

//    void makeMeshData()  ;

//    std::shared_ptr<detail::MeshData> data_ = nullptr ;

};

}

#endif
