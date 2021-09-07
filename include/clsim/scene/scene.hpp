#ifndef CLSIM_SCENE_SCENE_HPP
#define CLSIM_SCENE_SCENE_HPP

#include <vector>
#include <clsim/scene/scene_fwd.hpp>
#include <clsim/scene/node.hpp>
#include <clsim/scene/camera.hpp>
#include <clsim/scene/raycaster.hpp>

#include <assimp/cimport.h>

namespace clsim {

class Scene: public Node {
public:
    Scene() = default ;
};

class SceneLoaderException: public std::runtime_error {
public:
    SceneLoaderException(const std::string &message, const std::string &fname):
        std::runtime_error(message + "(" + fname  + ")") {}
};


}
#endif // CLSIM_SCENE_SCENE_HPP
