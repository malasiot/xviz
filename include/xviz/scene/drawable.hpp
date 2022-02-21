#ifndef XVIZ_SCENE_DRAWABLE_HPP
#define XVIZ_SCENE_DRAWABLE_HPP

#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/material.hpp>

namespace xviz {

template<typename T>
class EventDispatcher {
public:

    template<class... Args>
    void notify(Args... args) {
        for( auto &l: listeners_ )
            l(args...) ;
    }

    void addListener(T l) { listeners_.push_back(l) ; }
private:

    std::vector<T> listeners_ ;
};

// a drawable is a combination of geometry and material

class Drawable {
public:

    Drawable(const GeometryPtr &geom, const MaterialPtr &material):
        geometry_(geom), material_(material) {}

    GeometryPtr geometry() const { return geometry_ ; }
    MaterialPtr material() const { return material_ ; }

    void setMaterial(MaterialPtr mat) {
        auto old_mat = material_ ;
        material_ = mat ;
        mc_dispatcher_.notify(old_mat, mat) ;
    }

    void setGeometry(GeometryPtr geom) {
        auto old_geom = geometry_ ;
        geometry_ = geom ;
        gc_dispatcher_.notify(old_geom, geom) ;
    }

    static Drawable makeAxes(float sz);
    static Drawable makeGrid(float sz, uint32_t divisions, const Eigen::Vector4f &clr1={0.2, 0.2, 0.2, 1},
                             const Eigen::Vector4f &clr2 = {0.8, 0.8, 0.8, 1});

    using GeometryChangedEvent = std::function<void(const GeometryPtr &, const GeometryPtr &)> ;
    void addEventListener(GeometryChangedEvent &&e) {gc_dispatcher_.addListener(e); }

    using MaterialChangedEvent = std::function<void(const MaterialPtr &, const MaterialPtr &)> ;
    void addEventListener(MaterialChangedEvent &&e) { mc_dispatcher_.addListener(e); }

private:

    GeometryPtr geometry_ ;
    MaterialPtr material_ ;
    EventDispatcher<GeometryChangedEvent> gc_dispatcher_ ;
    EventDispatcher<MaterialChangedEvent> mc_dispatcher_ ;
};

} // namespace xviz
#endif
