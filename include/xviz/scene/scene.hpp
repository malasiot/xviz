#ifndef XVIZ_SCENE_SCENE_HPP
#define XVIZ_SCENE_SCENE_HPP

#include <vector>
#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/node.hpp>
#include <xviz/scene/camera.hpp>
#include <xviz/scene/raycaster.hpp>

#include <assimp/cimport.h>

namespace xviz {

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
