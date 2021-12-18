#ifndef XVIZ_SCENE_NODE_HELPERS_HPP
#define XVIZ_SCENE_NODE_HELPERS_HPP

#include <xviz/scene/node.hpp>
#include <xviz/scene/geometry.hpp>

namespace xviz {
class NodeHelpers {
public:
    static NodePtr makeAxes(float sz);
    static NodePtr makeGrid(float sz, uint32_t divisions, const Eigen::Vector4f &clr1, const Eigen::Vector4f &clr2);
    static NodePtr makeBox(const Eigen::Vector3f &hs, const Eigen::Vector4f &clr) ;
    static NodePtr makeCylinder(float r, float h, const Eigen::Vector4f &clr) ;
    static NodePtr makeSphere(float r,  const Eigen::Vector4f &clr) ;

    static NodePtr makeCircle(const Eigen::Vector3f &center, const Eigen::Vector3f &normal, float radius, const Eigen::Vector3f &color) ;
    static NodePtr makeLineString(const std::vector<Eigen::Vector3f> &pts, const Eigen::Vector3f &clr) ;
    static NodePtr makeArc(const Eigen::Vector3f &center, const Eigen::Vector3f &normal, const Eigen::Vector3f &axis, float radiusA, float radiusB, float minAngle, float maxAngle, const Eigen::Vector3f &color, bool drawSect, float stepDegrees);
};

}
#endif
