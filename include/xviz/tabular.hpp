#ifndef XVIZ_TABULAR_HPP
#define XVIZ_TABULAR_HPP

#include <vector>
#include <cstdint>
#include <memory>

namespace xviz {

// Hierarchical data structure to store column of arbitrary type

class Tabular {

private:
    class Data {
        int d ;
    };

    class Node {
    public:
        Node(const std::vector<Data> &data, Node *parent = nullptr): row_(-1), data_(data), parent_(parent) {}
        void addChildNode(Node *c) {
            c->row_ = children_.size() ;
            c->parent_ = this ;
            children_.push_back(c) ;
        }

        Node *child(int row) {
            if (row < 0 || row >= children_.size()) return nullptr;
            return children_.at(row);
        }

        const std::vector<Node *> &children() const { return children_ ; }

        int row() const { return row_ ; }

        const std::vector<Data> &columns() const { return data_ ; }

    private:
        int row_ = 0 ;
        std::vector<Data> data_ ; // column data
        Node *parent_ ; // parent node in case of hierarchical data
        std::vector<Node *> children_ ;
    };

    Tabular() {
        root_.reset(new Node({})) ;
    }

    Node *getNode(int row, Node *parent) {
        if ( parent == nullptr ) return root_->child(row) ;
        else return parent->child(row) ;
    }

    Node *addNode(const std::vector<Data> &data, Node *parent = nullptr) {
        Node *p = ( parent == nullptr ) ? root_.get() : parent ;
        Node *n = new Node(data, parent) ;
        p->addChildNode(n) ;
        nodes_.emplace_back(std::unique_ptr<Node>(n)) ;
    }

    std::unique_ptr<Node> root_ ;
    std::vector<std::unique_ptr<Node>> nodes_ ;
};

}

#endif
