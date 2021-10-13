#include "spatial_hashing.hpp"

using namespace Eigen ;

namespace xviz {

#define FLOOR(x) ( (int)(x + 32768.f) - 32768 )

SpatialHashingGrid::SpatialHashingGrid(float cell_size): cell_size_(cell_size) {

}

Vector3i SpatialHashingGrid::index(const Vector3f &p) const {
    int ix = FLOOR(p.x() / cell_size_) ;
    int iy = FLOOR(p.y() / cell_size_) ;
    int iz = FLOOR(p.z() / cell_size_) ;

    return {ix, iy, iz} ;
}

SpatialHashingGrid::Cell *SpatialHashingGrid::query(const Vector3i &i)
{
    auto it = hash_table_.find(i) ;
    return ( it == hash_table_.end() ) ? nullptr : &it->second ;
}


}
