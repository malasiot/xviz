#ifndef XVIZ_SCENE_GEOMETRY_HPP
#define XVIZ_SCENE_GEOMETRY_HPP

#include <xviz/scene/camera.hpp>
#include <xviz/scene/node.hpp>
#include <xviz/scene/detail/intersect.hpp>

namespace xviz {

class RayCastResult ;

namespace impl {
    class MeshData ;
}
const int MAX_TEXTURES = 2 ;

class Geometry {
public:
    enum PrimitiveType { Triangles, Lines, Points } ;

    using indices_t = std::vector<uint32_t> ;

    Geometry(): ptype_(Triangles) {}
    Geometry(PrimitiveType t): ptype_(t) {}
    ~Geometry() ;
    Geometry(const Geometry &) = default ;
    Geometry(Geometry &&) = default ;

    using vb3_t = std::vector<Eigen::Vector3f> ;
    using vb2_t = std::vector<Eigen::Vector2f> ;

    vb3_t &vertices() { return vertices_ ; }
    vb3_t &normals() { return normals_ ; }
    vb3_t &colors() { return colors_ ; }
    vb2_t &texCoords(unsigned int t) {
        assert(t<MAX_TEXTURES) ;
        return tex_coords_[t] ;
    }

    indices_t &indices() { return indices_ ; }
    const indices_t &indices() const { return indices_ ; }

    const vb3_t &vertices() const { return vertices_ ; }
    const vb3_t &normals() const { return normals_ ; }
    const vb3_t &colors() const { return colors_ ; }
    const vb2_t &texCoords(unsigned int t) const {
        assert ( t < MAX_TEXTURES ) ;
        return tex_coords_[t] ;
    }

    static const int MAX_BONES_PER_VERTEX = 4 ;

    struct BoneWeight {
        int32_t bone_[MAX_BONES_PER_VERTEX] ;
        float weight_[MAX_BONES_PER_VERTEX] ;

        BoneWeight() {
            std::fill(bone_, bone_ + MAX_BONES_PER_VERTEX, -1) ;
            std::fill(weight_, weight_ + MAX_BONES_PER_VERTEX, 0.f) ;
        }

        void add(int32_t bone, float w) {
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

    struct Bone {
        std::string name_ ;
        Eigen::Affine3f offset_ ;
        NodePtr node_ ;
    };

    int numUVChannels() const { return MAX_TEXTURES ; }

    bool castsShadows() const { return casts_shadows_ ; }
    void setCastsShadows(bool c) { casts_shadows_ = c ; }

    const std::vector<Bone> &skeleton() const { return skeleton_ ; }
    std::vector<Bone> &skeleton() { return skeleton_ ; }

    Eigen::Affine3f &skeletonInverseGlobalTransform() { return skeleton_inverse_global_transform_ ; }

    const std::vector<BoneWeight> &weights() const { return weights_ ; }
    std::vector<BoneWeight> &weights() { return weights_ ; }

    bool hasSkeleton() const { return !skeleton_.empty() ; }

    PrimitiveType ptype() const { return ptype_ ; }

    // primitive shape factories

    static Geometry createWireCube(const Eigen::Vector3f &hs) ;
    static Geometry createSolidCube(const Eigen::Vector3f &hs) ;

    static Geometry createWireSphere(float radius, size_t slices, size_t stacks) ;
    static Geometry createSolidSphere(float radius, size_t slices, size_t stacks) ;

    // the base of the cone is on (0, 0, 0) aligned with the z-axis and pointing towards positive z

    static Geometry createWireCone(float radius, float height, size_t slices, size_t stacks) ;
    static Geometry createSolidCone(float radius, float height, size_t slices, size_t stacks) ;

    static Geometry createWireCylinder(float radius, float height, size_t slices, size_t stacks) ;
    static Geometry createSolidCylinder(float radius, float height, size_t slices, size_t stacks, bool add_caps = true) ;

    static Geometry createSolidTorus(float R, float r, size_t n_sides, size_t n_rings);

    static Geometry createCapsule(float radius, float height, size_t slices, size_t head_stacks, size_t body_stacks) ;

    static Geometry makePointCloud(const std::vector<Eigen::Vector3f> &pts) ;
    static Geometry makePointCloud(const std::vector<Eigen::Vector3f> &coords,
                                   const std::vector<Eigen::Vector3f> &clrs) ;

    static Geometry makePlane(const float width, const float height, uint32_t nx=1 , uint32_t ny = 1) ;

    static Geometry makeArc(const Eigen::Vector3f& center, const Eigen::Vector3f& normal, const Eigen::Vector3f& axis,
                            float radiusA, float radiusB,
                            float minAngle, float maxAngle, bool drawSect, float stepDegrees) ;

    static Geometry makeCircle(const Eigen::Vector3f &center, const Eigen::Vector3f &normal, float radius, unsigned int num_segments);

    void computeNormals() ;
    detail::AABB getBoundingBox() ;
    void computeBoundingBox(Eigen::Vector3f &bmin, Eigen::Vector3f &bmax) const ;

    virtual bool hasCheapIntersectionTest() const { return false ; }
    virtual bool intersect(const Ray &, float &) const { return false ; }

    // this is the expensive test visiting all triangles of the geometry
    bool intersectTriangles(const Ray &, uint32_t tidx[3], float &t, bool back_face_culling) const ;
    bool intersectLines(const Ray &, uint32_t tidx[2], float line_thresh_sq, float &t) const ;

    void setVerticesUpdated(bool state) {
        vertices_updated_ = state ;
        if ( state ) box_.reset(nullptr) ;
    }

    void setNormalsUpdated(bool state) {
        normals_updated_ = state ;
    }

    void setColorsUpdated(bool state) {
        colors_updated_ = state ;
    }

    bool verticesUpdated() const { return vertices_updated_ ; }
    bool normalsUpdated() const { return normals_updated_ ; }
    bool colorsUpdated() const { return colors_updated_ ; }

private:

    friend class Renderer ;

    impl::MeshData *getMeshData() ;

    vb3_t vertices_, normals_, colors_ ;
    vb2_t tex_coords_[MAX_TEXTURES] ;
    indices_t indices_ ;
    std::vector<BoneWeight> weights_ ;
    std::vector<Bone> skeleton_ ;
    Eigen::Affine3f skeleton_inverse_global_transform_ = Eigen::Affine3f::Identity() ;
    bool casts_shadows_ = true ;
    bool vertices_updated_ = false, normals_updated_ = false, colors_updated_ = false ;
    std::unique_ptr<detail::AABB> box_ ;
    PrimitiveType ptype_ = Triangles ;

    impl::MeshData *data_ = nullptr ;
};

class BoxGeometry: public Geometry {
public:
    BoxGeometry(const Eigen::Vector3f &he): half_extents_(he),
        Geometry(std::move(Geometry::createSolidCube(he))) { }

    ~BoxGeometry() {}

    Eigen::Vector3f halfExtents() const { return half_extents_ ; }

    bool hasCheapIntersectionTest() const override { return true ; }
    bool intersect(const Ray &, float &) const override ;

private:

    Eigen::Vector3f half_extents_ ;
};

class SphereGeometry: public Geometry {
public:
    SphereGeometry(float radius, size_t slices = 12, size_t stacks = 10): radius_(radius), slices_(slices),
        stacks_(stacks),
        Geometry(std::move(Geometry::createSolidSphere(radius, slices, stacks))) { }

    float radius() const { return radius_ ; }
    size_t slices() const { return slices_ ; }
    size_t stacks() const { return stacks_ ; }

    bool hasCheapIntersectionTest() const override { return true ; }
    bool intersect(const Ray &, float &) const override ;

private:

    float radius_ ;
    size_t slices_, stacks_ ;
};

// cylinder center at origin aligned with Y axis. Height is half length.

class CylinderGeometry: public Geometry {
public:

    CylinderGeometry(float r, float h,  size_t slices = 12, size_t stacks = 10): radius_(r), height_(h),
        slices_(slices), stacks_(stacks),
        Geometry(std::move(Geometry::createSolidCylinder(r, h, slices, stacks))) {}

    float radius() const { return radius_ ; }
    float height() const { return height_ ; }

    size_t slices() const { return slices_ ; }
    size_t stacks() const { return stacks_ ; }

    bool hasCheapIntersectionTest() const override { return true ; }
    bool intersect(const Ray &, float &) const override ;

private:

    float radius_, height_ ;
    size_t slices_, stacks_ ;
};

class ConeGeometry: public Geometry {
public:
    ConeGeometry(float r, float h): radius_(r), height_(h) {}

    float radius() const { return radius_ ; }
    float height() const { return height_ ; }

    bool hasCheapIntersectionTest() const override { return true ; }
    bool intersect(const Ray &, float &) const override ;

private:
    float radius_, height_ ;
};

}

#endif
