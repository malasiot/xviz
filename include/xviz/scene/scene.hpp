#ifndef XVIZ_SCENE_SCENE_HPP
#define XVIZ_SCENE_SCENE_HPP

#include <vector>
#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/node.hpp>
#include <assimp/cimport.h>

namespace xviz {

namespace msg {
class Scene ;
}

class Scene {
public:
    Scene() = default ;

    enum { IMPORT_ANIMATIONS = 0x1, IMPORT_SKELETONS = 0x2, IMPORT_LIGHTS = 0x4 } ;

    void load(const std::string &fname, int flags = 0, float scale = 1.f ) ;
    void load(const aiScene *sc, const std::string &fname, int flags = 0, float scale = 1.f) ;

    void addNode(NodePtr node) {
        nodes_.push_back(node) ;
    }

    void addLight(LightPtr l) {
        NodePtr node(new Node) ;
        node->setLight(l) ;
        nodes_.push_back(node) ;
    }

    const std::vector<Geometry *> geometries() const ;
    const std::vector<NodePtr> &nodes() const { return nodes_ ; }
    const std::vector<Material *> materials() const ;
    const std::vector<LightPtr> lights() const ;

    void visitNodes(const std::function<void(const Node &n)> &f) const{
        for( const auto &node: nodes_ ) {
            if ( !node->parent() ) Node::visit(node, f) ;
        }
    }

    Eigen::Vector3f geomCenter() const ;
    float geomRadius(const Eigen::Vector3f &center) const ;

    static msg::Scene *write(const Scene &scene) ;
    static Scene *read(const msg::Scene &msg) ;

private:

    std::vector<NodePtr> nodes_ ;
    std::vector<LightPtr> lights_ ;
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
