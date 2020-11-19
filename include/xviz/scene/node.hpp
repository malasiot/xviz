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

    void setDrawable(const Drawable *) ;

    void addChild(const NodePtr &n) {
        children_.push_back(n) ;
        n->parent_ = this ;
    }

    void setTransform(const Eigen::Affine3f &tr) {
        mat_ = tr ;
    }

    Node *parent() const { return parent_ ; }

    const Drawable *drawable() const { return drawable_.get() ; }

    const std::vector<NodePtr> &children() const { return children_ ; }

    size_t numChildren() const { return children_.size() ; }

    NodePtr getChild(size_t idx) const {
        assert( idx < children_.size() ) ;
        return children_[idx] ;
    }

    void setLight(const LightPtr &light) { light_ = light ; }

    LightPtr light() const { return light_ ; }

    Eigen::Affine3f globalTransform() const {
        if ( parent_ ) return parent_->globalTransform() * mat_ ;
        else return mat_ ;
    }

    NodePtr makeChildNode(const MeshPtr &geom, const MaterialPtr &mat) ;

private:

    std::string name_ ;

    Eigen::Affine3f mat_ ;             // transformation matrix to apply to child nodes and attached geometries

    std::vector<NodePtr> children_ ;      // child nodes

    LightPtr light_ ;
    std::unique_ptr<Drawable> drawable_ ;

    Node *parent_ = nullptr;
};

class NodeVisitor {
public:
    NodeVisitor() = default ;

    virtual void visit(Node &node) = 0 ;

    void visitChildren(Node &n) {
        for( auto &c: n.children() ) {
            visit(*c) ;
        }
    }
};

class ConstNodeVisitor {
public:
    ConstNodeVisitor() = default ;

    virtual void visit(const Node &node) = 0 ;

    void visitChildren(const Node &n) {
        for( auto c: n.children() ) {
            visit(*c) ;
        }
    }
};

}

#endif
