#ifndef XVIZ_SCENE_SCENE_HPP
#define XVIZ_SCENE_SCENE_HPP

#include <vector>
#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/node.hpp>
#include <assimp/cimport.h>

namespace xviz {

class Scene {
public:
    Scene() = default ;

    enum { IMPORT_ANIMATIONS = 0x1, IMPORT_SKELETONS = 0x2, IMPORT_LIGHTS = 0x4 } ;

    void load(const std::string &fname, int flags = 0, float scale = 1.f ) ;
    void load(const aiScene *sc, const std::string &fname, int flags = 0, float scale = 1.f) ;

    void addNode(NodePtr node) {
        nodes_.push_back(node) ;
    }

    void addMesh(MeshPtr mesh) {
        meshes_.push_back(mesh) ;
    }

    void addMaterial(MaterialPtr mat) {
        materials_.push_back(mat) ;
    }

    void addLight(LightPtr l) {
        lights_.push_back(l) ;
    }

    const std::vector<MeshPtr> &meshes() const { return meshes_ ; }
    const std::vector<NodePtr> &nodes() const { return nodes_ ; }
    const std::vector<MaterialPtr> &materials() const { return materials_ ; }
    const std::vector<LightPtr> &lights() const { return lights_ ; }

    void visitNodes(const std::function<void(const Node &n)> &f) const{
        for( const auto &node: nodes_ ) {
            if ( !node->parent() ) Node::visit(node, f) ;
        }
    }

    Eigen::Vector3f geomCenter() const ;
    float geomRadius(const Eigen::Vector3f &center) const ;

private:

    std::vector<NodePtr> nodes_ ;
    std::vector<MeshPtr> meshes_ ;
    std::vector<MaterialPtr> materials_ ;
    std::vector<LightPtr> lights_ ;
};

class SceneLoaderException: public std::runtime_error {
public:
    SceneLoaderException(const std::string &message, const std::string &fname):
        std::runtime_error(message + "(" + fname  + ")") {}
};


}
#endif // XVIZ_SCENE_SCENE_HPP
