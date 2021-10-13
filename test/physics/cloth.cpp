#include "cloth.hpp"

#include <map>

using namespace Eigen ;
using namespace std ;

namespace xviz {

void Cloth::reset(unsigned int n_iter) {
    for( auto &p: particles_ ) {
       p.x_ = p.x0_ ;
       p.p_ = p.x_ ;
       p.v_ = { 0.f, 0.f, 0.f } ;
       p.w_ = p.mass_ == 0.0f ? 0.0 : 1.0f/p.mass_ ;
       p.f_ = { 0.f, 0.f, 0.f } ;
    }

    for( auto &c: distance_constraints_ ) {
        c.init(n_iter) ;
    }

    for( auto &c: bending_constraints_ ) {
        c.init(n_iter) ;
    }
}

void Cloth::getMesh(std::vector<Vector3f> &vertices, std::vector<Vector3f> &normals, std::vector<uint32_t> &indices)
{
    std::map<Particle *, unsigned int> idxs ;

    for( unsigned int i=0 ; i<particles_.size() ; i++ ) {
        auto &p = particles_[i] ;
        Vector3f v = p.x_ ;
        vertices.push_back(v) ;
        idxs[&p] = i ;
    }

    if ( indices.empty() ) {
        for( unsigned int i=0 ; i< faces_.size() ; i++ ) {
            const auto &face = faces_[i] ;
            unsigned int idx0 = face.v0_ ;
            unsigned int idx1 = face.v1_ ;
            unsigned int idx2 = face.v2_ ;
            indices.push_back(idx0) ;
            indices.push_back(idx1) ;
            indices.push_back(idx2) ;
        }
    }


}

RectangularPatch::RectangularPatch(float mass, const Eigen::Vector3f &c00, const Eigen::Vector3f &c10, const Eigen::Vector3f &c01,
                                   unsigned int nvX, unsigned int nvY, unsigned int flags, float k_stretch, float k_bend)
{
#define IDX(_x_, _y_) ((_y_)*nvX + (_x_))
    /* Create nodes	*/
    assert( nvX >=2 && nvY >=2) ;
    unsigned int numNodes = nvX * nvY ;

    particles_.resize(numNodes) ;

    float stepX = (c00 - c10).norm() / (float)(nvX - 1) ;
    float stepY = (c00 - c01).norm() / (float)(nvY - 1) ;

    Vector3f deltaX = (c10 - c00).normalized() * stepX ;
    Vector3f deltaY = (c01 - c00).normalized() * stepY ;

    Vector3f py = c00 ;
    for ( unsigned int iy = 0; iy < nvY; ++iy, py += deltaY ) {
        Vector3f px = py ;
        for ( unsigned int ix = 0; ix < nvX; ++ix, px += deltaX ) {
            auto &p = particles_[IDX(ix, iy)] ;
            p.x0_ = px ;
            p.mass_ = mass/numNodes ;
        }
    }

    if (flags & TopLeft ) particles_[IDX(0, 0)].mass_ = 0 ;
    if (flags & TopRight ) particles_[IDX(nvX - 1, 0)].mass_ = 0;
    if (flags & BottomLeft) particles_[IDX(0, nvY - 1)].mass_ = 0;
    if (flags & BottomRight ) particles_[IDX(nvX - 1, nvY - 1)].mass_ = 0;

    if ( flags & TopEdge ) {
        for( unsigned int x=0 ; x<nvX ; x++ )
            particles_[IDX(x, 0)].mass_ = 0 ;
    }

    if ( flags & BottomEdge ) {
        for( unsigned int x=0 ; x<nvX ; x++ )
            particles_[IDX(x, nvY-1)].mass_ = 0 ;
    }

    if ( flags & LeftEdge ) {
        for( unsigned int y=0 ; y<nvY ; y++ )
            particles_[IDX(0, y)].mass_ = 0 ;
    }

    if ( flags & RightEdge ) {
        for( unsigned int y=0 ; y<nvY ; y++ )
            particles_[IDX(nvX-1, y)].mass_ = 0 ;
    }

    /* Create links	and faces */
    for ( unsigned int iy = 0; iy < nvY; ++iy) {
        for (unsigned int ix = 0; ix < nvX; ++ix) {
            const unsigned int idx = IDX(ix, iy);
            const bool mdx = (ix + 1) < nvX;
            const bool mdy = (iy + 1) < nvY;
            if (mdx) distance_constraints_.emplace_back(particles_[idx], particles_[IDX(ix + 1, iy)], k_stretch);
            if (mdy) distance_constraints_.emplace_back(particles_[idx], particles_[IDX(ix, iy + 1)], k_stretch);

            if (mdx && mdy) {
                if ((ix + iy) & 1)
                {
                    faces_.emplace_back(IDX(ix, iy), IDX(ix + 1, iy), IDX(ix + 1, iy + 1));
                    faces_.emplace_back(IDX(ix, iy), IDX(ix + 1, iy + 1), IDX(ix, iy + 1));
                    distance_constraints_.emplace_back(particles_[IDX(ix, iy)], particles_[IDX(ix + 1, iy + 1)], k_stretch );
                }
                else
                {
                    faces_.emplace_back(IDX(ix, iy + 1), IDX(ix, iy), IDX(ix + 1, iy));
                    faces_.emplace_back(IDX(ix, iy + 1), IDX(ix + 1, iy), IDX(ix + 1, iy + 1));
                    distance_constraints_.emplace_back(particles_[IDX(ix + 1, iy)], particles_[IDX(ix, iy + 1)], k_stretch );
                }



            }
        }
    }

}

}
