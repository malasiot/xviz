#include <xviz/tabular.hpp>

#include "session.pb.h"

namespace xviz {

Tabular::Tabular(const std::vector<TabularColumn> &columns): columns_(columns),  root_(new TabularNode({})) {}

Tabular::~Tabular() {
    delete root_ ;
}

TabularNode *Tabular::getNode(int row, const TabularNode *parent) {
    if ( parent == nullptr ) return root()->child(row) ;
    else return parent->child(row) ;
}

TabularNode *Tabular::addNode(const std::vector<TabularData> &data, TabularNode *parent) {
    TabularNode *p = ( parent == nullptr ) ? root_ : parent ;

    TabularNode *c = new TabularNode(data, p) ;

    p->addChildNode(c) ;

    return c ;
}

static void write_node(const TabularNode *n, msg::TabularNode *msg) {
    const auto &columns = n->columns() ;
    for( const auto &col: columns ) {
        msg::TabularData *data_msg = msg->add_columns() ;
        switch ( col.type() ) {
        case TabularData::Type::Boolean:
            data_msg->set_b(col.toBoolean()) ;
            break ;
        case TabularData::Type::String:
            data_msg->set_s(col.toString()) ;
            break ;
        case TabularData::Type::SignedInteger:
            data_msg->set_i(col.toSignedInteger()) ;
            break ;
        case TabularData::Type::UnsignedInteger:
            data_msg->set_u(col.toUnsignedInteger()) ;
            break ;
        case TabularData::Type::Float:
            data_msg->set_f(col.toFloat()) ;
            break ;
        case TabularData::Type::Double:
            data_msg->set_f(col.toDouble()) ;
            break ;
        default:
            break ;
        }
    }

    for( TabularNode *child: n->children() ) {
         msg::TabularNode *child_msg = msg->add_children() ;
         write_node(child, child_msg) ;
    }
}

std::string Tabular::write(const Tabular &t) {
    msg::Tabular tab ;
    msg::TabularNode *r = new msg::TabularNode ;
    write_node(t.root(), r) ;
    tab.set_allocated_root(r) ;

    for( const auto &col: t.columns() ) {
        msg::TabularColumn *msg_col = tab.add_columns() ;
        msg_col->set_id(col.id()) ;
        if ( !col.name().empty() )
            msg_col->set_header(col.name()) ;
    }

    return tab.SerializeAsString();
}

static void read_node(Tabular *t, const msg::TabularNode &msg, TabularNode *parent) {
    std::vector<TabularData> data ;

    for( const msg::TabularData &col: msg.columns() ) {

        switch (col.data_case()) {
        case msg::TabularData::kS:
            data.emplace_back(col.s()) ;
            break ;
        case msg::TabularData::kU:
            data.emplace_back(col.u()) ;
            break ;
        case msg::TabularData::kI:
            data.emplace_back(col.i()) ;
            break ;
        case msg::TabularData::kB:
            data.emplace_back(col.b()) ;
            break ;
        case msg::TabularData::kF:
            data.emplace_back(col.f()) ;
            break ;
        case msg::TabularData::kD:
            data.emplace_back(col.d()) ;
            break ;
        default:
            data.emplace_back() ;

        }
    }

    TabularNode *node = t->addNode(data, parent) ;

    for( const auto &child: msg.children() ) {
        read_node(t, child, node) ;
    }
}

Tabular *Tabular::read(const std::string &msg) {
    msg::Tabular tab ;
    if ( !tab.ParseFromString(msg) ) return nullptr ;

    std::vector<TabularColumn> columns ;

    for ( const msg::TabularColumn &col: tab.columns() ) {
        columns.emplace_back(col.id(), col.header()) ;
    }

    Tabular *t = new Tabular(columns) ;

    const msg::TabularNode &root = tab.root() ;
    for( const msg::TabularNode &child: root.children()) {
        read_node(t, child, nullptr);
    }

    return t ;
}

TabularNode::TabularNode(const std::vector<TabularData> &data, TabularNode *parent): row_(-1), data_(data), parent_(parent) {}

TabularNode::~TabularNode() {
    for( TabularNode *child: children_ )
        delete child ;
}

void TabularNode::addChildNode(TabularNode *c) {
    c->row_ = children_.size() ;
    c->parent_ = this ;
    children_.push_back(c) ;
}



}
