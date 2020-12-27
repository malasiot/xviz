#ifndef XVIZ_SCENE_NODE_HELPERS_HPP
#define  XVIZ_SCENE_NODE_HELPERS_HPP

#include <xviz/scene/node.hpp>
#include <xviz/scene/geometry.hpp>

namespace xviz {
class NodeHelpers {
public:
    static NodePtr makeAxes(float sz);
    static NodePtr makeGrid(float sz, uint32_t divisions, const Eigen::Vector4f &clr1, const Eigen::Vector4f &clr2);
};

}
#endif
