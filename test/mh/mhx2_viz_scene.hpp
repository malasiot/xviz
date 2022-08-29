#ifndef MH_SCENE_HPP
#define MH_SCENE_HPP

#include "mhx2.hpp"

#include <xviz/scene/scene.hpp>

class MHNode: public xviz::Node {
public:
    MHNode(const MHX2Model &model) ;

private:
    void createGeometry(const MHX2Model &model) ;
};

#endif
