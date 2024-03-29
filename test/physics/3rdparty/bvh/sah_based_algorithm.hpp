#ifndef BVH_SAH_BASED_ALGORITHM_HPP
#define BVH_SAH_BASED_ALGORITHM_HPP

#include "bvh.hpp"

namespace bvh {

template <typename Bvh>
class SahBasedAlgorithm {
    using Scalar = typename Bvh::ScalarType;

public:
    /// Cost of intersecting a ray with a node of the data structure.
    /// This cost is relative to the cost of intersecting a primitive,
    /// which is assumed to be equal to 1.
    Scalar traversal_cost = 1;

protected:
    ~SahBasedAlgorithm() {}

    Scalar compute_cost(const Bvh& bvh) const {
        // Compute the SAH cost for the entire BVH
        Scalar cost(0);
        #pragma omp parallel for reduction(+: cost)
        for (size_t i = 0; i < bvh.node_count; ++i) {
            if (bvh.nodes[i].is_leaf())
                cost += bvh.nodes[i].bounding_box_proxy().half_area() * bvh.nodes[i].primitive_count;
            else
                cost += traversal_cost * bvh.nodes[i].bounding_box_proxy().half_area();
        }
        return cost / bvh.nodes[0].bounding_box_proxy().half_area();
    }
};

} // namespace bvh

#endif
