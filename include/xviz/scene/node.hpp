#ifndef XVIZ_SCENE_NODE_HPP
#define XVIZ_SCENE_NODE_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

#include <Eigen/Geometry>

#include <xviz/scene/scene_fwd.hpp>
#include <xviz/scene/drawable.hpp>
#include <xviz/scene/animation.hpp>

class aiScene ;

namespace xviz {

// a hieracrchy of nodes. each node applies a transformation to the attached geometries, cameras, lights

class Node: public std::enable_shared_from_this<Node> {
public:

    using NodePtr = std::shared_ptr<Node> ;


    enum { IMPORT_ANIMATIONS = 0x1, IMPORT_SKELETONS = 0x2, IMPORT_LIGHTS = 0x4 } ;

    void load(const std::string &fname, int flags = 0 ) ;
    void load(const aiScene *sc, const std::string &fname, int flags = 0) ;


    Node() { mat_.setIdentity() ; }
    virtual ~Node() = default;


    const Eigen::Affine3f &transform() const { return mat_ ; }
    Eigen::Affine3f &transform() { return mat_ ; }

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
    std::vector<NodePtr> &children() { return children_ ; }

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

    template<typename F>
    void visit(F f) const {
        for( const NodePtr &c: children() ) {
            c->visit(f) ;
        }
        f(*this) ;
    }

    template<typename F>
    void visit(F f) {
        for( auto &c: children() ) {
            c->visit(f) ;
        }
        f(*this) ;
    }

    std::vector<ConstNodePtr> getNodesRecursive() const {
        std::vector<ConstNodePtr> nodes ;
        visit([&](const Node &n) {
            nodes.push_back(n.shared_from_this()) ;
        }) ;
        return nodes ;
    }

    std::vector<NodePtr> getNodesRecursive() {
        std::vector<NodePtr> nodes ;
        visit([&](Node &n) {
            nodes.push_back(n.shared_from_this()) ;
        }) ;
        return nodes ;
    }

    std::vector<NodePtr> getOrderedNodes() {
        std::vector<NodePtr> nodes = getNodesRecursive() ;
        std::sort(nodes.begin(), nodes.end(), [](const NodePtr &a, const NodePtr &b ) { return a->order_ < b->order_ ; }) ;
        return nodes ;
    }

    void setOrder(int order, bool recursive = true) {
        order_ = order ;
        if ( recursive ) {
            for( auto &c: children_ )
                c->setOrder(order) ;
        }
    }

    void addAnimation(Animation *anim) { animations_.emplace_back(anim) ; }

    void updateAnimations(float t) {
        for( auto &a: animations_ )
            a->update(t) ;

        for( auto &c: children_ )
            c->updateAnimations(t) ;
    }

    void startAnimations(float t) {
        for( auto &a: animations_ )
            a->start(t) ;

        for( auto &c: children_ )
            c->startAnimations(t) ;
    }

    void stopAnimations() {
        for( auto &a: animations_ )
            a->stop() ;

        for( auto &c: children_ )
            c->stopAnimations() ;
    }

    void updateTransforms(const std::map<std::string, Eigen::Isometry3f> &trs) {
        for( const auto &tp: trs ) {
            auto node = findNodeByName(tp.first) ;
            if ( node ) {
                node->setTransform(tp.second) ;
            }
        }
    }



    Eigen::Vector3f geomCenter() const ;
    float geomRadius(const Eigen::Vector3f &center) const ;

    NodePtr findNodeByName(const std::string &name) ;

    void setVisible(bool v, bool recursive = true) {
        visible_ = v ;
        if ( recursive ) {
            for( auto &c: children_ )
                c->setVisible(v) ;
        }
    }

    bool isVisible() const { return visible_ ; }

private:

    std::string name_ ;

    Eigen::Affine3f mat_ ;             // transformation matrix to apply to child nodes and attached geometries

    using children_t = std::vector<NodePtr> ;

    children_t children_ ;      // child nodes

    LightPtr light_ ;
    std::vector<Drawable> drawables_ ;

    std::vector<std::unique_ptr<Animation>> animations_ ;

    Node *parent_ = nullptr;

    bool visible_ = true ;

    int order_ = 0 ;
};


}

#endif
