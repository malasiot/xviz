#ifndef XVIZ_SCENE_DRAWABLE_HPP
#define XVIZ_SCENE_DRAWABLE_HPP

#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/material.hpp>

namespace xviz {

// a drawable is a combination of geometry and material

class Drawable {
public:

    Drawable(const GeometryPtr &geom, const MaterialPtr &material):
        geometry_(geom), material_(material) {}

    GeometryPtr geometry() const { return geometry_ ; }
    MaterialPtr material() const { return material_ ; }

    void setMaterial(MaterialPtr mat) { material_ = mat ; }
    void setGeometry(GeometryPtr geom) { geometry_ = geom ; }

    static Drawable makeAxes(float sz);
    static Drawable makeGrid(float sz, uint32_t divisions, const Eigen::Vector4f &clr1={0.2, 0.2, 0.2, 1},
                             const Eigen::Vector4f &clr2 = {0.8, 0.8, 0.8, 1});

private:

    GeometryPtr geometry_ ;
    MaterialPtr material_ ;
};

} // namespace xviz
#endif
