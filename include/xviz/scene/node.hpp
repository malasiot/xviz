#ifndef XVIZ_SCENE_NODE_HPP
#define XVIZ_SCENE_NODE_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

#include <Eigen/Geometry>

#include <assimp/scene.h>

#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/drawable.hpp>
namespace xviz {

// a hieracrchy of nodes. each node applies a transformation to the attached geometries, cameras, lights


class Node: public std::enable_shared_from_this<Node> {
public:

    using NodePtr = std::shared_ptr<Node> ;

    Node() { mat_.setIdentity() ; }

    Eigen::Affine3f &matrix() { return mat_ ; }

    std::string name() const { return name_ ; }

    void setName(const std::string &name) { name_ = name ; }

    void addDrawable(const Drawable &d) { drawables_.emplace_back(d) ; }
    void addDrawable(const GeometryPtr &geom, const MaterialPtr &material) {
        drawables_.emplace_back(geom, material) ;
    }

    void addChild(const NodePtr &n) {
        children_.push_back(n) ;
        n->parent_ = this ;
    }

    void setTransform(const Eigen::Affine3f &tr) {
        mat_ = tr ;
    }

    Node *parent() const { return parent_ ; }

    const std::vector<Drawable> &drawables() const { return drawables_ ; }
    std::vector<Drawable> &drawables() { return drawables_ ; }

    const std::vector<NodePtr> &children() const { return children_ ; }

    size_t numChildren() const { return children_.size() ; }

    NodePtr getChild(size_t idx) const {
        assert( idx < children_.size() ) ;
        return children_[idx] ;
    }

    void setLight(const LightPtr &light) { light_ = light ; }

    LightPtr light() const { return light_ ; }


    void addLightNode(LightPtr l) {
        NodePtr node(new Node) ;
        node->setLight(l) ;
        addChild(node) ;
    }

    Eigen::Affine3f globalTransform() const {
        if ( parent_ ) return parent_->globalTransform() * mat_ ;
        else return mat_ ;
    }

    NodePtr makeChildNode(const MeshPtr &geom, const MaterialPtr &mat) ;

    const std::vector<Geometry *> geometries() const ;
    const std::vector<Material *> materials() const ;
    const std::vector<LightPtr> lights() const ;

    void visitNodes(const std::function<void(const NodePtr &n)> &f) const{
        for( const auto &node: children_ ) {
            if ( !node->parent() ) Node::visit(node, f) ;
        }
    }

    static void visit(const NodePtr &parent, const std::function<void(const NodePtr&)> &f) {
        for( const auto &c: parent->children() ) {
            Node::visit(c, f) ;
        }
        f(parent) ;
    }

    void addScene(const ScenePtr &other);

private:

    std::string name_ ;

    Eigen::Affine3f mat_ ;             // transformation matrix to apply to child nodes and attached geometries

    using children_t = std::vector<NodePtr> ;

    children_t children_ ;      // child nodes

    LightPtr light_ ;
    std::vector<Drawable> drawables_ ;

    Node *parent_ = nullptr;
};


}

#endif
