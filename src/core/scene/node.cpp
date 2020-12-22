#include <xviz/scene/node.hpp>
#include <xviz/scene/scene.hpp>

namespace xviz {

void Node::addScene(const xviz::ScenePtr &other) {
    for( const NodePtr &n: other->nodes() )
        if ( n->parent() == nullptr ) addChild(n) ;
}

}
