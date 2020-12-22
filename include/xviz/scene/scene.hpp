#ifndef XVIZ_SCENE_SCENE_HPP
#define XVIZ_SCENE_SCENE_HPP

#include <vector>
#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/node.hpp>
#include <xviz/scene/camera.hpp>
#include <xviz/scene/raycaster.hpp>

#include <assimp/cimport.h>

namespace xviz {

namespace msg {
class Scene ;
}

class Scene: public Node {
public:
    Scene() = default ;

    static msg::Scene *write(const Scene &scene) ;
    static Scene *read(const msg::Scene &msg) ;

private:

};

class SceneLoaderException: public std::runtime_error {
public:
    SceneLoaderException(const std::string &message, const std::string &fname):
        std::runtime_error(message + "(" + fname  + ")") {}
};

enum class SceneMessageType { Url, Data, Empty } ;

class SceneMessage: public Message {
public:

    SceneMessage(): type_(SceneMessageType::Empty) {}
    SceneMessage(const std::string &url): type_(SceneMessageType::Url), url_(url) {}
    SceneMessage(Scene *scene): type_(SceneMessageType::Data), scene_(scene) {}

    SceneMessageType type() const { return type_ ; }

    ScenePtr scene() { return scene_ ; }
    const std::string &url() const { return url_ ; }

    std::string encode() const override ;
    static SceneMessage *decode(const std::string &msg) ;

private:
    std::string url_ ;
    ScenePtr scene_ ;
    SceneMessageType type_ = SceneMessageType::Empty ;
};

}
#endif // XVIZ_SCENE_SCENE_HPP
