#include <xviz/scene/mesh.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/raycaster.hpp>
#include <xviz/scene/detail/octree.hpp>
#include <xviz/scene/detail/intersect.hpp>

#include "renderer/mesh_data.hpp"

#include <fstream>

using namespace Eigen ;
using namespace std ;

namespace xviz {


static Geometry flatten(const std::vector<Vector3f> &vertices, const std::vector<uint32_t> &vtx_indices,
                        const std::vector<Vector3f> &normals, const std::vector<uint32_t> &nrm_indices,
                        const std::vector<Vector2f> &uvs = {}, const std::vector<uint32_t> &uv_indices = {}) {

    // check if we need to flatten otherwise return src Mesh

    Geometry dst ;

    auto &dst_vertices = dst.vertices() ;
    auto &dst_normals = dst.normals() ;
    auto &dst_uvs = dst.texCoords(0) ;

    size_t n_indices = vtx_indices.size() ;

    for( unsigned int i=0 ; i<n_indices ; i++) {

        uint32_t vidx = vtx_indices[i] ;
        const Vector3f &pos = vertices[vidx] ;
        dst_vertices.push_back(pos) ;

        if ( !nrm_indices.empty() ) {
            uint32_t nidx = nrm_indices[i] ;
            const Vector3f &normal = normals[nidx] ;
            dst_normals.push_back(normal) ;
        } else if ( !normals.empty() ) {
            const Vector3f &norm = normals[vidx] ;
            dst_normals.push_back(norm) ;
        }

        if ( !uv_indices.empty() ) {
            uint32_t nidx = uv_indices[i] ;
            const Vector2f &uv = uvs[nidx] ;
            dst_uvs.push_back(uv) ;
        } else if ( !uvs.empty() ) {
            const Vector2f &uv = uvs[vidx] ;
            dst_uvs.push_back(uv) ;
        }
    }

    return dst ;
}

Geometry::~Geometry() {
}

Geometry Geometry::createWireCube(const Vector3f &hs) {

    Geometry m(Lines) ;

    m.vertices() =
    {{ -hs.x(), +hs.y(), +hs.z() }, { +hs.x(), +hs.y(), +hs.z() }, { +hs.x(), -hs.y(), +hs.z() }, { -hs.x(), -hs.y(), +hs.z() },
    { -hs.x(), +hs.y(), -hs.z() }, { +hs.x(), +hs.y(), -hs.z() }, { +hs.x(), -hs.y(), -hs.z() }, { -hs.x(), -hs.y(), -hs.z() } } ;
    m.indices() =  {  0, 1, 1, 2, 2, 3, 3, 0,  4, 5, 5, 6, 6, 7,  7, 4, 0, 4, 1, 5, 2, 6, 3, 7 };

    return m ;
}

Geometry Geometry::createSolidCube(const Vector3f &hs) {

    std::vector<Vector3f> normals{{ 0.0, 0.0, 1.0 }, {0.0, 0.0, -1.0}, { 0.0, 1.0, 0.0 }, {1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, { -1.0, 0.0, 0.0}} ;
    std::vector<Vector3f> vertices = {{ -hs.x(), +hs.y(), +hs.z() }, { +hs.x(), +hs.y(), +hs.z() }, { +hs.x(), -hs.y(), +hs.z() }, { -hs.x(), -hs.y(), +hs.z() },
                                      { -hs.x(), +hs.y(), -hs.z() }, { +hs.x(), +hs.y(), -hs.z() }, { +hs.x(), -hs.y(), -hs.z() }, { -hs.x(), -hs.y(), -hs.z() } } ;

    std::vector<uint32_t> vtx_indices {  1, 0, 3,  7, 4, 5,  4, 0, 1,  5, 1, 2,  2, 3, 7,  0, 4, 7,  1, 3, 2,  7, 5, 6,  4, 1, 5,  5, 2, 6,  2, 7, 6, 0, 7, 3};
    std::vector<uint32_t> nrm_indices {  0, 0, 0,  1, 1, 1,  2, 2, 2,  3, 3, 3,  4, 4, 4,  5, 5, 5,  0, 0, 0,  1, 1, 1,  2, 2, 2,  3, 3, 3,  4, 4, 4, 5, 5, 5};

    std::vector<Vector2f> uv{ { 0.0, 0.0 },  {1.0, 0.0},  {1.0, 1.0}, {0.0, 1.0} };
    std::vector<uint32_t> uv_indices { 1, 0, 3, 1, 3, 2, 1, 0, 3, 1, 3, 2, 1, 0, 3, 1, 3, 2, 1, 0, 3, 1, 3, 2, 1, 0, 3, 1, 3, 2, 1, 0, 3, 1, 3, 2 } ;
    return flatten(vertices, vtx_indices, normals, nrm_indices, uv, uv_indices) ;
}

// adapted from freeglut

static void makeCircleTable(vector<float> &sint, vector<float> &cost, int n, bool half_circle = false) {

    /* Table size, the sign of n flips the circle direction */

    const size_t size = abs(n);

    /* Determine the angle between samples */

    const float angle = (half_circle ? 1 : 2)*M_PI/(float)( ( n == 0 ) ? 1 : n );

    sint.resize(size+1) ; cost.resize(size+1) ;

    /* Compute cos and sin around the circle */

    sint[0] = 0.0;
    cost[0] = 1.0;

    for ( size_t i =1 ; i<size; i++ ) {
        sint[i] = sin(angle*i);
        cost[i] = cos(angle*i);
    }

    /* Last sample is duplicate of the first */

    sint[size] = sint[0];
    cost[size] = cost[0];
}

Geometry Geometry::createSolidCone(float radius, float height, size_t slices, size_t stacks)
{
    float z0,z1;
    float r0,r1;

    const float zStep = height / std::max(stacks, (size_t)1) ;
    const float rStep = radius / std::max(stacks, (size_t)1) ;

    const float cosn = ( height / sqrt ( height * height + radius * radius ));
    const float sinn = ( radius / sqrt ( height * height + radius * radius ));

    vector<float> sint, cost;
    makeCircleTable( sint, cost, slices);

    /* Cover the circular base with a triangle fan... */

    z0 = 0.0;
    z1 = z0+ zStep;

    r0 = radius ;
    r1 = r0 - rStep;

    // the base of the cone is on (0, 0, 0) aligned with the z-axis and pointing towards positive z

    std::vector<Vector3f> vertices, normals ;
    std::vector<uint32_t> vtx_indices, nrm_indices ;

    vertices.push_back({0, 0, z0}) ;
    normals.push_back({0, 0, -1}) ;

    for( unsigned int i=0 ; i<slices ; i++ ) {
        vertices.push_back({cost[i]*r0, sint[i]*r0, z0}) ;
    }

    for( unsigned int i=0 ; i<slices ; i++ ) {

        vtx_indices.push_back(i+1) ;
        vtx_indices.push_back(0) ;
        vtx_indices.push_back(i == slices-1 ? 1 : i+2) ;

        nrm_indices.push_back(0) ;
        nrm_indices.push_back(0) ;
        nrm_indices.push_back(0) ;
    }

    // normals shared by all side vertices

    for( unsigned int i=0 ; i<slices ; i++ ) {
        normals.push_back({cost[i]*sinn, sint[i]*sinn, cosn}) ;
    }

    for( size_t j = 1;  j < stacks; j++ ) {

        for( unsigned int i=0 ; i<slices ; i++ ) {
            vertices.push_back({cost[i]*r1, sint[i]*r1, z1}) ;
        }

        for( unsigned int i=0 ; i<slices ; i++ ) {
            size_t pn = ( i == slices - 1 ) ? 0 : i+1 ;
            vtx_indices.push_back((j-1)*slices + i + 1) ;
            vtx_indices.push_back((j-1)*slices + pn + 1) ;
            vtx_indices.push_back((j)*slices + pn + 1) ;

            vtx_indices.push_back((j-1)*slices + i + 1) ;
            vtx_indices.push_back((j)*slices + pn + 1) ;
            vtx_indices.push_back((j)*slices + i + 1) ;

            nrm_indices.push_back(i + 1) ;
            nrm_indices.push_back(pn + 1) ;
            nrm_indices.push_back(pn + 1) ;

            nrm_indices.push_back(i + 1) ;
            nrm_indices.push_back(pn + 1) ;
            nrm_indices.push_back(i + 1) ;
        }

        z1 += zStep;
        r1 -= rStep ;
    }

    // link apex with last stack

    size_t offset = (stacks - 1)*slices + 1;

    vertices.push_back({0, 0, z1}) ;

    for( unsigned int i=0 ; i<slices ; i++ ) {
        size_t pn = ( i == slices - 1 ) ? 0 : i+1 ;
        vtx_indices.push_back(offset + i) ;
        vtx_indices.push_back(offset + pn) ;
        vtx_indices.push_back(offset + slices) ;

        nrm_indices.push_back(i + 1) ;
        nrm_indices.push_back(pn + 1) ;
        nrm_indices.push_back(i + 1) ;
    }

    return flatten(vertices, vtx_indices, normals, nrm_indices, {}, {}) ;
}

Geometry Geometry::createWireCone(float radius, float height, size_t slices, size_t stacks)
{
    Geometry m(Lines) ;

    float z0,z1;
    float r0;

    vector<float> sint, cost;
    makeCircleTable( sint, cost, slices);

    z0 = 0.0;
    r0 = radius ;
    z1 = z0 + height ;

    for( unsigned int i=0 ; i<slices ; i++ ) {
        m.vertices().push_back({cost[i]*r0, sint[i]*r0, z0}) ;
    }

    m.vertices().push_back({0, 0, z1}) ;

    for( unsigned int i=0 ; i<slices-1 ; i++ ) {
        m.indices().push_back(i) ;
        m.indices().push_back(i+1) ;
    }
    m.indices().push_back(slices-1) ;
    m.indices().push_back(0) ;

    for( unsigned int i=0 ; i<slices ; i++ ) {
        m.indices().push_back(i) ;
        m.indices().push_back(slices) ;
    }

    return m ;
}

Geometry Geometry::createSolidCylinder(float radius, float height, size_t slices, size_t stacks, bool add_caps)
{

    float z0,z1;

    const float zStep = height / std::max(stacks, (size_t)1) ;

    vector<float> sint, cost;
    makeCircleTable( sint, cost, slices);

    /* Cover the circular base with a triangle fan... */

    z0 = -height/2.0;
    z1 = z0 + zStep;

    vector<Vector3f> vertices, normals ;
    vector<uint32_t> vtx_indices, nrm_indices ;

    unsigned int co = 0 ;

    if ( add_caps ) {
        vertices.push_back({0, 0, z0}) ;
        normals.push_back({0, 0, -1}) ;
        co = 1 ;
    }

    for( unsigned int i=0 ; i<slices ; i++ ) {
        vertices.push_back({cost[i]*radius, sint[i]*radius, z0}) ;
    }

    if ( add_caps ) {
        for( unsigned int i=0 ; i<slices ; i++ ) {

            vtx_indices.push_back(i+1) ;
            vtx_indices.push_back(0) ;
            vtx_indices.push_back(i == slices-1 ? 1 : i+2) ;

            nrm_indices.push_back(0) ;
            nrm_indices.push_back(0) ;
            nrm_indices.push_back(0) ;
        }
    }


    // normals shared by all side vertices

    for( unsigned int i=0 ; i<slices ; i++ ) {
        normals.push_back({cost[i], sint[i], 1.0}) ;
    }

    for( size_t j = 1 ;  j <= stacks; j++ ) {

        for( unsigned int i=0 ; i<slices ; i++ ) {
            vertices.push_back({cost[i]*radius, sint[i]*radius, z1}) ;
        }

        for( unsigned int i=0 ; i<slices ; i++ ) {
            size_t pn = ( i == slices - 1 ) ? 0 : i+1 ;
            vtx_indices.push_back((j-1)*slices + i + co) ;
            vtx_indices.push_back((j-1)*slices + pn + co) ;
            vtx_indices.push_back((j)*slices + pn + co) ;

            vtx_indices.push_back((j-1)*slices + i + co) ;
            vtx_indices.push_back((j)*slices + pn + co) ;
            vtx_indices.push_back((j)*slices + i + co) ;

            nrm_indices.push_back(i + co) ;
            nrm_indices.push_back(pn + co) ;
            nrm_indices.push_back(pn + co) ;

            nrm_indices.push_back(i + co) ;
            nrm_indices.push_back(pn + co) ;
            nrm_indices.push_back(i + co) ;
        }

        z1 += zStep;
    }

    // link apex with last stack

    size_t offset = (stacks)*slices + co;

    if ( add_caps ) {
        vertices.push_back({0.f, 0.f, height/2.0f}) ;
        normals.push_back({0, 0, 1}) ;

        for( unsigned int i=0 ; i<slices ; i++ ) {
            size_t pn = ( i == slices - 1 ) ? 0 : i+1 ;
            vtx_indices.push_back(offset + i) ;
            vtx_indices.push_back(offset + pn) ;
            vtx_indices.push_back(offset + slices) ;

            nrm_indices.push_back(slices+1) ;
            nrm_indices.push_back(slices+1) ;
            nrm_indices.push_back(slices+1) ;
        }
    }

    return flatten(vertices, vtx_indices, normals, nrm_indices) ;
}

Geometry Geometry::createCapsule(float radius, float height, size_t slices, size_t head_stacks, size_t body_stacks)
{

    // make cylinder

    float z0,z1;

    const float zStep = (height - 2*radius) / std::max(body_stacks, (size_t)1) ;

    vector<float> sint, cost;
    makeCircleTable( sint, cost, slices);

    z0 = -height/2.0f - radius ;
    z1 = z0 + zStep;

    vector<Vector3f> vertices, normals ;
    vector<uint32_t> vtx_indices, nrm_indices ;

    for( unsigned int i=0 ; i<slices ; i++ ) {
        vertices.push_back({cost[i]*radius, sint[i]*radius, z0}) ;
    }

    // normals shared by all side vertices

    for( unsigned int i=0 ; i<slices ; i++ ) {
        normals.push_back({cost[i], sint[i], 1.0}) ;
    }

    for( size_t j = 1;  j <= body_stacks; j++ ) {

        for( unsigned int i=0 ; i<slices ; i++ ) {
            vertices.push_back({cost[i]*radius, sint[i]*radius, z1}) ;
        }

        for( unsigned int i=0 ; i<slices ; i++ ) {
            size_t pn = ( i == slices - 1 ) ? 0 : i+1 ;
            vtx_indices.push_back((j-1)*slices + i) ;
            vtx_indices.push_back((j-1)*slices + pn) ;
            vtx_indices.push_back((j)*slices + pn) ;

            vtx_indices.push_back((j-1)*slices + i) ;
            vtx_indices.push_back((j)*slices + pn) ;
            vtx_indices.push_back((j)*slices + i) ;

            nrm_indices.push_back(i) ;
            nrm_indices.push_back(pn) ;
            nrm_indices.push_back(pn) ;

            nrm_indices.push_back(i) ;
            nrm_indices.push_back(pn) ;
            nrm_indices.push_back(i) ;

        }

        z1 += zStep;
    }

    return flatten(vertices, vtx_indices, normals, nrm_indices) ;

}

Geometry Geometry::createSolidTorus(float R, float r, size_t n_sides, size_t n_rings) {

     float phi, theta1;
     float cos_theta1, sin_theta1;

     float ring_delta = 2.0 * M_PI / n_rings;
     float side_delta = 2.0 * M_PI / n_sides;

     float theta = 0.0;
     float cos_theta = 1.0;
     float sin_theta = 0.0;

     Vector3f pv1, pv2, pn1, pn2 ;
     int c = 0 ;
     bool first = true ;

     vector<Vector3f> vertices, normals ;
     vector<uint32_t> vtx_indices, nrm_indices ;

     for ( int i = n_rings - 1; i >= 0; i-- ) {
       theta1 = theta + ring_delta;
       cos_theta1 = cos(theta1);
       sin_theta1 = sin(theta1);

       phi = 0.0;
       for ( int j = n_sides; j >= 0; j-- ) {
         float cos_phi, sin_phi, dist;

         phi += side_delta;
         cos_phi = cos(phi);
         sin_phi = sin(phi);
         dist = R + r * cos_phi;

         Vector3f n1 { cos_theta1 * cos_phi, -sin_theta1 * cos_phi, sin_phi } ;
         Vector3f v1 { cos_theta1 * dist, -sin_theta1 * dist, r * sin_phi } ;

         Vector3f n2 { cos_theta * cos_phi, -sin_theta * cos_phi, sin_phi } ;
         Vector3f v2 { cos_theta * dist, -sin_theta * dist, r * sin_phi } ;

         vertices.push_back(v2) ;
         vertices.push_back(v1) ;

         normals.push_back(n2) ;
         normals.push_back(n1) ;

         if ( !first ) {
             vtx_indices.push_back(c) ;
             vtx_indices.push_back(c+3) ;
             vtx_indices.push_back(c+1) ;

             vtx_indices.push_back(c) ;
             vtx_indices.push_back(c+2) ;
             vtx_indices.push_back(c+3) ;

             nrm_indices.push_back(c) ;
             nrm_indices.push_back(c+3) ;
             nrm_indices.push_back(c+1) ;

             nrm_indices.push_back(c) ;
             nrm_indices.push_back(c+2) ;
             nrm_indices.push_back(c+3) ;

             c += 2 ;
         }

         pv1 = v1 ; pv2 = v2 ;
         pn1 = n1 ; pn2 = n2 ;
         first = false ;
       }
       theta = theta1;
       cos_theta = cos_theta1;
       sin_theta = sin_theta1;
     }

     return flatten(vertices, vtx_indices, normals, nrm_indices) ;

}

Geometry Geometry::makePointCloud(const std::vector<Vector3f> &pts) {
    Geometry m(Points) ;
    m.vertices() = pts ;
    return m ;
}

Geometry Geometry::makePointCloud(const std::vector<Vector3f>  &coords, const std::vector<Vector3f>  &clrs) {

    Geometry m(Points) ;

    m.vertices() = coords ;
    m.colors() = clrs ;

    return m ;
}

Geometry Geometry::makePlane(const float width, const float height, uint32_t nx, uint32_t ny)
{
    float stepx = width/nx ;
    float stepy = height/ny ;

    float stepu = 1.0/nx ;
    float stepv = 1.0/ny ;

    Vector3f c(-width/2, 0, -height/2) ;

    Geometry geom ;

    for( unsigned int i=0 ; i<ny+1 ; i++ )
        for( unsigned int j=0 ; j<nx+1 ; j++ ) {
            Vector3f v =  c + Vector3f(j* stepx, 0, i*stepy) ;
            Vector2f uv(j*stepu, 1.0f - i*stepv) ;

            geom.vertices().push_back(v) ;
            geom.texCoords(0).push_back(uv) ;
            geom.normals().push_back({0, 1, 0}) ;
        }

    for ( unsigned int i = 0; i < ny; i ++ ) {
        for ( unsigned int j = 0; j < nx ; j++ ) {
            uint32_t a = j + (nx+1) * i;
            uint32_t b = j + (nx+1) * ( i + 1 );
            uint32_t c = ( j + 1 ) + (nx+1) * ( i + 1 );
            uint32_t d = ( j + 1 ) + (nx+1) * i;

            geom.indices().push_back(a) ;
            geom.indices().push_back(b) ;
            geom.indices().push_back(d) ;

            geom.indices().push_back(b) ;
            geom.indices().push_back(c) ;
            geom.indices().push_back(d) ;
        }

    }

    return geom ;
}

Geometry Geometry::makeArc(const Vector3f &center, const Vector3f &normal, const Vector3f &axis, float radiusA, float radiusB, float minAngle, float maxAngle, bool drawSect, float stepDegrees)
{

    vector<Vector3f> ls ;

    Vector3f vy = normal.cross(axis);
    vy.normalize() ;
    Vector3f vx = vy.cross(normal) ;

    float step = stepDegrees * M_PI/180.0;
    int nSteps = (int)fabs((maxAngle - minAngle) / step);
    if ( nSteps == 0 ) nSteps = 1;
    Vector3f prev = center + radiusA * vx * cos(minAngle) + radiusB * vy * sin(minAngle);

    if (drawSect)
        ls.emplace_back(center) ;

    ls.emplace_back(prev) ;

    for ( int i = 1; i <= nSteps; i++ )
    {
        float angle = minAngle + (maxAngle - minAngle) * i / float(nSteps);
        Vector3f next = center + radiusA * vx * cos(angle) + radiusB * vy * sin(angle);
        ls.emplace_back(next) ;
    }

    if (drawSect)
        ls.emplace_back(center) ;

    Geometry m(Geometry::Lines) ;
    m.vertices() = ls ;
    return m ;
}

Geometry Geometry::makeCircle(const Vector3f &center, const Vector3f &normal, float radius, unsigned int num_segments) {
    Vector3f axis = ( fabs(normal.y()) < std::numeric_limits<float>::min() ) ? Vector3f(normal.z(), 0, -normal.x()) : Vector3f(normal.y(), -normal.x(), normal.z()) ;
    return makeArc(center, normal, axis, radius, radius, 0, 2*M_PI, false, 2* M_PI/num_segments) ;
}

static Vector3f normal_triangle(const Vector3f &v1, const Vector3f &v2, const Vector3f &v3)
{
    Vector3f n1, n2 ;

    n1 = v1 - v2 ;
    n2 = v1 - v3 ;
    return  n1.cross(n2).normalized() ;

}

void Geometry::computeNormals() {

    size_t n = vertices().size() ;
    normals_.resize(n) ;

    for( unsigned int i=0 ; i<n ; i++ ) normals_.data()[i] = Vector3f::Zero() ;

    for( unsigned int i=0 ; i<indices_.size() ; i+=3 )
    {
        unsigned int idx0 = indices_[i] ;
        unsigned int idx1 = indices_[i+1] ;
        unsigned int idx2 = indices_[i+2] ;
        Vector3f n = normal_triangle(vertices().data()[idx0], vertices().data()[idx1], vertices().data()[idx2]) ;

        normals_[idx0] += n ;
        normals_[idx1] += n ;
        normals_[idx2] += n ;
    }

    for( unsigned int i=0 ; i<n ; i++ ) normals_[i].normalize() ;
}

detail::AABB Geometry::getBoundingBox() {
    if ( !box_ ) {
        Vector3f bmin, bmax ;
        computeBoundingBox(bmin, bmax) ;
        box_.reset(new detail::AABB{bmin, bmax}) ;
    }
    return *box_ ;
}



void Geometry::computeBoundingBox(Vector3f &vmin, Vector3f &vmax) const {

    assert( !vertices_.empty() ) ;

    vmin = vmax = vertices_[0] ;

    for( const Vector3f &v: vertices_ ) {
        vmin.x() = std::min(vmin.x(), v.x()) ;
        vmin.y() = std::min(vmin.y(), v.y()) ;
        vmin.z() = std::min(vmin.z(), v.z()) ;
        vmax.x() = std::max(vmax.x(), v.x()) ;
        vmax.y() = std::max(vmax.y(), v.y()) ;
        vmax.z() = std::max(vmax.z(), v.z()) ;
    }
}


bool Geometry::intersectTriangles(const Ray &ray, uint32_t t_idx[3], float &bestt, bool back_face_culling) const
{
    float mint = std::numeric_limits<float>::max() ;
    bool hit = false ;


    if ( !indices_.empty() ) {
        for( unsigned int i=0, ti =0 ; i<indices_.size() ; i+=3, ti++ ) {
            uint32_t idx0 = indices_[i] ;
            uint32_t idx1 = indices_[i+1] ;
            uint32_t idx2 = indices_[i+2] ;

            const Vector3f &v0 = vertices_[idx0] ;
            const Vector3f &v1 = vertices_[idx1] ;
            const Vector3f &v2 = vertices_[idx2] ;

            float t ;
            if ( detail::rayIntersectsTriangle(ray, v0, v1, v2, back_face_culling, t)  && t < mint  ) {
                mint = t ;
                bestt = t ;
                t_idx[0] = idx0 ; t_idx[1] = idx1 ; t_idx[2] = idx2 ;
                hit = true ;
            }
        }
    } else {
        for( unsigned int i=0, ti =0 ; i<vertices_.size() ; i+=3, ti++ ) {
            uint32_t idx0 = i ;
            uint32_t idx1 = i+1 ;
            uint32_t idx2 = i+2 ;

            const Vector3f &v0 = vertices_[idx0] ;
            const Vector3f &v1 = vertices_[idx1] ;
            const Vector3f &v2 = vertices_[idx2] ;

            float t ;
            if ( detail::rayIntersectsTriangle(ray, v0, v1, v2, back_face_culling, t)  && t < mint  ) {
                mint = t ;
                bestt = t ;
                t_idx[0] = idx0 ; t_idx[1] = idx1 ; t_idx[2] = idx2 ;
                hit = true ;
            }
        }
    }

    return hit ;
}

bool Geometry::intersectLines(const Ray &ray, uint32_t t_idx[2], float thresh_sq, float &bestt) const
{
    float mint = std::numeric_limits<float>::max() ;
    bool hit = false ;


    if ( !indices_.empty() ) {
        for( unsigned int i=0, ti =0 ; i<indices_.size() ; i+=2, ti++ ) {
            uint32_t idx0 = indices_[i] ;
            uint32_t idx1 = indices_[i+1] ;

            const Vector3f &v0 = vertices_[idx0] ;
            const Vector3f &v1 = vertices_[idx1] ;

            float t ;
            if ( detail::rayIntersectsLine(ray, v0, v1, thresh_sq, t)  && t < mint  ) {
                mint = t ;
                bestt = t ;
                t_idx[0] = idx0 ; t_idx[1] = idx1 ;
                hit = true ;
            }
        }
    } else {
        for( unsigned int i=0, ti =0 ; i<vertices_.size() ; i+=3, ti++ ) {
            uint32_t idx0 = i ;
            uint32_t idx1 = i+1 ;

            const Vector3f &v0 = vertices_[idx0] ;
            const Vector3f &v1 = vertices_[idx1] ;

            float t ;
            if ( detail::rayIntersectsLine(ray, v0, v1, thresh_sq, t)  && t < mint  ) {
                mint = t ;
                bestt = t ;
                t_idx[0] = idx0 ; t_idx[1] = idx1 ;
                hit = true ;
            }
        }
    }

    return hit ;
}


Geometry Geometry::createWireCylinder(float radius, float height, size_t slices, size_t stacks)
{
    Geometry m(Lines) ;

    float z0,z1;

    const float zStep = height / std::max(stacks, (size_t)1) ;

    vector<float> sint, cost;
    makeCircleTable( sint, cost, slices);

    z0 = 0.0;


    for( unsigned int i=0 ; i<slices ; i++ ) {
        m.vertices().push_back({cost[i]*radius, sint[i]*radius, z0}) ;
    }

    for( unsigned int i=0 ; i<slices ; i++ ) {
        m.vertices().push_back({cost[i]*radius, sint[i]*radius, z0 + height}) ;
    }

    for( unsigned int i=0 ; i<slices-1 ; i++ ) {
        m.indices().push_back(i) ;
        m.indices().push_back(i+1) ;
    }
    m.indices().push_back(slices-1) ;
    m.indices().push_back(0) ;

    unsigned int offset = slices ;

    for( unsigned int i=0 ; i<slices-1 ; i++ ) {
        m.indices().push_back(offset + i) ;
        m.indices().push_back(offset + i+1) ;
    }
    m.indices().push_back(offset + slices-1) ;
    m.indices().push_back(offset) ;

    for( unsigned int i=0 ; i<slices ; i++ ) {
        m.indices().push_back(i) ;
        m.indices().push_back(i + offset) ;
    }

    return m ;
}


static void exportToObj(const std::string &fname, const std::vector<Vector3f> &vertices, const std::vector<Vector3f> &normals, const std::vector<unsigned int> &indices) {
    ofstream strm(fname) ;

    for( unsigned int i=0 ;i<vertices.size() ; i++ ) {
        strm << "v " << vertices[i].adjoint() << endl ;
    }

    for( unsigned int i=0 ;i<normals.size() ; i++ ) {
        strm << "vn " << normals[i].adjoint() << endl ;
    }

    for( unsigned int i=0 ; i<indices.size() ; i+=3) {
        strm << "f " << indices[i] + 1 << ' ' << indices[i+1] +1<< ' ' << indices[i+2] + 1<< endl ;
    }
}

Geometry Geometry::createSolidSphere(float radius, size_t slices, size_t stacks) {

    Geometry m(Triangles) ;

    int idx = 0;
    float x,y,z;
    int n_vertices ;

    /* Pre-computed circle */
    vector<float> sint1, cost1, sint2, cost2;

    /* number of unique vertices */
    assert (slices !=0 && stacks > 1 );

    n_vertices = slices*(stacks-1) + 2 ;

    makeCircleTable(sint1, cost1, -slices, false) ;
    makeCircleTable(sint2, cost2, stacks, true) ;

    m.vertices().resize(n_vertices) ;
    m.normals().resize(n_vertices) ;

    /* top */

    auto &&vertices = m.vertices() ;
    auto &&normals = m.normals() ;
    auto &&indices = m.indices() ;

    vertices[0] = { 0.f, 0.f, radius } ;
    normals[0] = { 0.f, 0.f, 1.0f } ;

    idx = 1;

    /* each stack */
    for( unsigned int i=1; i<stacks; i++ )
    {
        for( unsigned int j=0; j<slices; j++, idx++)
        {
            x = cost1[j]*sint2[i];
            y = sint1[j]*sint2[i];
            z = cost2[i];

            vertices[idx] = { x*radius, y*radius, z*radius } ;
            normals[idx] = { x, y, z } ;
        }
    }

    vertices[idx] = { 0.0f, 0.0f, -radius } ;
    normals[idx] = { 0.0f, 0.0f, -1.0f } ;

    indices.resize(6*slices + 6*(stacks-2)*slices) ;

    /* top stack */

    idx = 0 ;
    for ( unsigned int j=0;  j<slices-1;  j++) {
        indices[idx++] = j+2 ;
        indices[idx++] = j+1 ;
        indices[idx++] = 0 ;
    }

    indices[idx++] = 1 ;
    indices[idx++] = slices ;
    indices[idx++] = 0 ;

    for ( unsigned int i=0; i< stacks-2; i++ )
    {
        unsigned int offset = 1+i*slices;                    /* triangle_strip indices start at 1 (0 is top vertex), and we advance one stack down as we go along */
        unsigned int j ;

        for ( j=0; j<slices-1; j++ ) {
            indices[idx++] = offset + j + slices ;
            indices[idx++] = offset + j ;
            indices[idx++] = offset + j + 1 ;

            indices[idx++] = offset + j + slices ;
            indices[idx++] = offset + j + 1;
            indices[idx++] = offset + j + slices + 1 ;
        }

        indices[idx++] = offset + slices ;
        indices[idx++] = offset + j + slices ;
        indices[idx++] = offset  ;

        indices[idx++] = offset  ;
        indices[idx++] = offset + j + slices ;
        indices[idx++] = offset + j ;

    }

    /* bottom stack */
    int offset = 1+(stacks-2)*slices;               /* triangle_strip indices start at 1 (0 is top vertex), and we advance one stack down as we go along */

    for ( unsigned int j=0;  j<slices-1;  j++) {
        indices[idx++] = j + offset  ;
        indices[idx++] = j + offset + 1;
        indices[idx++] = n_vertices-1 ;
    }

    indices[idx++] = offset + slices - 1 ;
    indices[idx++] = offset ;
    indices[idx++] = n_vertices-1 ;

    //    exportToObj("/tmp/Mesh.obj", vertices, normals, indices) ;

    return m ;
}



using namespace detail ;

bool BoxGeometry::intersect(const Ray &ray, float &t) const {
    AABB box(-half_extents_, half_extents_) ;
    return rayIntersectsAABB(ray, box, t) ;
}

bool SphereGeometry::intersect(const Ray &ray, float &t) const {
    return rayIntersectsSphere(ray, {0, 0, 0}, radius_, t) ;
}

bool CylinderGeometry::intersect(const Ray &ray, float &t) const {
    return rayIntersectsCylinder(ray, radius_, height_, t) ;
}
}
