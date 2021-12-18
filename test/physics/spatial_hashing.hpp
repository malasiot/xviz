#ifndef SPATIAL_HASHING_HPP
#define SPATIAL_HASHING_HPP

#include <Eigen/Geometry>
#include <unordered_map>

namespace xviz {
    class SpatialHashingCell ;
}
#if 0
namespace std {
    template<>
    struct hash<clsim::SpatialHashingCell>
 {
   std::size_t operator()(const Key& k) const
   {
     using std::size_t;
     using std::hash;
     using std::string;

     // Compute individual hash values for first,
     // second and third and combine them using XOR
     // and bit shifting:

     return ((hash<string>()(k.first)
              ^ (hash<string>()(k.second) << 1)) >> 1)
              ^ (hash<int>()(k.third) << 1);
   }
 };

    inline unsigned int hashFunction<NeighborhoodSearchCellPos*>(NeighborhoodSearchCellPos* const &key)
    {
        const int p1 = 73856093 * (*key)[0];
        const int p2 = 19349663 * (*key)[1];
        const int p3 = 83492791 * (*key)[2];
        return p1 + p2 + p3;
    }
}
#endif
namespace xviz {



struct KeyHasher {
    size_t operator()(const Eigen::Vector3i& k) const
    {
        const int p1 = 73856093 * k.x();
        const int p2 = 19349663 * k.y();
        const int p3 = 83492791 * k.z();
        return p1 + p2 + p3;
    }
};


class SpatialHashingGrid {
public:
    SpatialHashingGrid(float cell_size) ;
private:
    struct Cell {
        std::vector<int32_t> triangles_ ;
    };

    // get cell index
    Eigen::Vector3i index(const Eigen::Vector3f &p) const ;

    Cell *query(const Eigen::Vector3i &i) ;

    std::unordered_map<Eigen::Vector3i, Cell, KeyHasher> hash_table_ ;
    float cell_size_ ;
};


}
#endif
