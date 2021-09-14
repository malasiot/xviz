#include <clsim/physics/collision_object.hpp>

#include "3rdparty/bvh/bvh.hpp"
#include "3rdparty/bvh/vector.hpp"
#include "3rdparty/bvh/triangle.hpp"
#include "3rdparty/bvh/sphere.hpp"
#include "3rdparty/bvh/ray.hpp"
#include "3rdparty/bvh/sweep_sah_builder.hpp"
#include "3rdparty/bvh/single_ray_traverser.hpp"
#include "3rdparty/bvh/primitive_intersectors.hpp"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <clsim/scene/geometry.hpp>
#include <iostream>

using namespace std ;

using Scalar   = float;
using Vector3  = bvh::Vector3<Scalar>;
using Triangle = bvh::Triangle<Scalar>;
using Sphere   = bvh::Sphere<Scalar>;
using Ray      = bvh::Ray<Scalar>;
using Bvh      = bvh::Bvh<Scalar>;

using namespace std ;

namespace clsim {

static Eigen::Vector3f bvh2eigen(const Vector3 &v) {
    return {v[0], v[1], v[2]} ;
}

static Vector3 eigen2bvh(const Eigen::Vector3f &v) {
    return {v[0], v[1], v[2]} ;
}

namespace impl {
class BVH {
public:
    BVH() ;
    ~BVH() ;
    void create(const std::vector<Eigen::Vector3f> &vertices, const std::vector<uint32_t> &indices) ;
    void create(const aiScene *) ;
    bool intersect(const Ray &ray, Eigen::Vector3f &v, Eigen::Vector3f &n, float &t) ;

    using Triangle = bvh::Triangle<float>;

    std::vector<Triangle> primitives_ ;
    bvh::Bvh<float> bvh_ ;
};

BVH::BVH() {}
BVH::~BVH() {}

void BVH::create(const std::vector<Eigen::Vector3f> &vertices, const std::vector<uint32_t> &indices)
{
    for( size_t i=0 ; i<indices.size() ; i+=3 ) {
        const Eigen::Vector3f &v0 = vertices[indices[i]] ;
        const Eigen::Vector3f &v1 = vertices[indices[i+1]] ;
        const Eigen::Vector3f &v2 = vertices[indices[i+2]] ;

        Triangle triangle(eigen2bvh(v0), eigen2bvh(v1), eigen2bvh(v2));

        primitives_.emplace_back(triangle) ;
    }

    auto bboxes_centers = bvh::compute_bounding_boxes_and_centers(primitives_.data(), primitives_.size());
    auto global_bbox = bvh::compute_bounding_boxes_union(bboxes_centers.first.get(), primitives_.size());

    bvh::SweepSahBuilder<Bvh> builder(bvh_);
    builder.build(global_bbox, bboxes_centers.first.get(), bboxes_centers.second.get(), primitives_.size());
}

bool BVH::intersect(const Ray &ray, Eigen::Vector3f &vi, Eigen::Vector3f &n, float &t) {
    bvh::ClosestPrimitiveIntersector<Bvh, Triangle> primitive_intersector(bvh_, primitives_.data());
    bvh::SingleRayTraverser<Bvh> traverser(bvh_);

    const Eigen::Vector3f &orig = ray.origin() ;
    const Eigen::Vector3f &dir = ray.dir() ;

    bvh::Ray bray(eigen2bvh(orig), eigen2bvh(dir) ) ;

    if ( auto hit = traverser.traverse(bray, primitive_intersector) ) {
        auto triangle_index = hit->primitive_index;
        auto intersection = hit->intersection;

        const auto &triangle = primitives_[triangle_index] ;

        t = intersection.t ;
        float u = intersection.u ;
        float v = intersection.v ;

      //  vi = bvh2eigen(triangle.p0) + u*bvh2eigen(triangle.e2) - v*bvh2eigen(triangle.e1) ;

        vi = orig + t * dir ;
        n = -bvh2eigen(triangle.n).normalized() ;
    /*

        std::cout
                << "Hit primitive " << triangle_index << "\n"
                    << "distance: "    << intersection.t << "\n";

        std::cout
                << "u: "           << intersection.u << "\n"
                        << "v: "           << intersection.v << "\n";
        */
        return true;

    }
    return false;
}

}

CollisionObject::~CollisionObject() {}

bool CollisionObject::intersect(const Ray &ray, Eigen::Vector3f &v, Eigen::Vector3f &n, float &t)
{
    return bvh_->intersect(ray, v, n, t) ;
}

void CollisionObject::makeVisual(const std::vector<Eigen::Vector3f> &vertices, const std::vector<uint32_t> &indices, const std::vector<Eigen::Vector3f> &normals)
{
    GeometryPtr geom(new Geometry(Geometry::Triangles)) ;
    geom->indices() = indices ;
    geom->vertices() = vertices ;
    geom->normals() = normals ;

    PhongMaterial *mat = new PhongMaterial({0.0f, 0.0f, 0.5f, 1.0f}) ;

    Drawable dr(geom, MaterialPtr(mat)) ;

    visual_.reset(new Node) ;
    visual_->addDrawable(dr) ;
}

CollisionObject::CollisionObject(const std::vector<Eigen::Vector3f> &vertices, const std::vector<uint32_t> &indices, const std::vector<Eigen::Vector3f> &normals): bvh_(new impl::BVH()) {
    bvh_->create(vertices, indices) ;
    makeVisual(vertices, indices, normals) ;
}


CollisionObject::CollisionObject(const std::string &path, const Eigen::Isometry3f &mat): bvh_(new impl::BVH()) {
    const aiScene *scene = aiImportFile(path.c_str(),
                                     aiProcess_PreTransformVertices
                                     | aiProcess_GenNormals
                                     | aiProcess_Triangulate
                                     | aiProcess_JoinIdenticalVertices
                                     | aiProcess_SortByPType
                                     | aiProcess_OptimizeMeshes
                                     ) ;
    if ( !scene ) return ;

    std::vector<uint32_t> indices ;
    std::vector<Eigen::Vector3f> vertices, normals ;

    uint32_t offset = 0 ;
    for(size_t i=0 ; i<scene->mNumMeshes ; i++) {
        aiMesh *mesh = scene->mMeshes[i] ;
        if ( mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE ) continue ;

        for( size_t j=0 ; j<mesh->mNumFaces ; j++ ) {
            aiFace &face = mesh->mFaces[j] ;
            indices.push_back(face.mIndices[0] + offset) ;
            indices.push_back(face.mIndices[1] + offset) ;
            indices.push_back(face.mIndices[2] + offset) ;
        }

        for( size_t j=0 ; j<mesh->mNumVertices ; j++ ) {
            aiVector3D &v = mesh->mVertices[j] ;

            Eigen::Vector3f vtx{v.x, v.y, v.z}, vt ;

            vt = mat * vtx ;
            vertices.emplace_back(vt) ;
        }

        for( size_t j=0 ; j<mesh->mNumVertices ; j++ ) {
            aiVector3D &v = mesh->mNormals[j] ;

            Eigen::Vector3f nrm{v.x, v.y, v.z}, nt ;

            nt = mat.rotation() * nrm ;
            normals.emplace_back(nt) ;
        }

        offset += mesh->mNumVertices ;
    }

    bvh_->create(vertices, indices) ;

    makeVisual(vertices, indices, normals) ;

}

}
