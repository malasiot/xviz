#include "tabular_element.hpp"

#include "image_element.hpp"
#include <xviz/tabular.hpp>

#include <session.pb.h>

using namespace std ;


TabularElement::TabularElement() {

}

void TabularElement::buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) {

    widget_ = new QTreeView(parent) ;

    widget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    parseChannels(ele, channels_) ;

    setup_flex(ele, widget_) ;
}


void TabularElement::updateState(const xviz::msg::StateUpdate &state_update) {
    string channel_id = state_update.channel_id();
    //string object_id = state_update.object_id() ;
    if ( !channels_.contains(QByteArray::fromStdString(channel_id))) return ;
    string data = state_update.data() ;

    xviz::Tabular *t = xviz::Tabular::read(data) ;

    if ( !t ) return ;

    model_ = new TabularDataModel(t) ;
    widget_->setModel(model_) ;
    widget_->update() ;

}


TabularDataModel::TabularDataModel(xviz::Tabular *data, QObject *parent): QAbstractItemModel(parent) {
    data_.reset(data) ;
}


QVariant TabularDataModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    /*
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);
*/
    return QVariant();
}
QModelIndex TabularDataModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    xviz::TabularNode *parentItem ;

    if (!parent.isValid())
        parentItem = data_->root() ;
    else
        parentItem = static_cast<xviz::TabularNode *>(parent.internalPointer());

    xviz::TabularNode *childItem = parentItem->child(row);

    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TabularDataModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    xviz::TabularNode *childItem = static_cast<xviz::TabularNode *>(index.internalPointer());
    xviz::TabularNode *parentItem = childItem->parent() ;

    if (parentItem == nullptr )
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TabularDataModel::rowCount(const QModelIndex &parent) const
{
    xviz::TabularNode *parentItem;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = data_->root() ;
    else
        parentItem = static_cast<xviz::TabularNode *>(parent.internalPointer());

    return parentItem->children().size();
}

int TabularDataModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return static_cast<xviz::TabularNode *>(parent.internalPointer())->columns().size();
    return num_columns_;
}

QVariant TabularDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    xviz::TabularNode *item = static_cast<xviz::TabularNode *>(index.internalPointer());

    const xviz::TabularData &val = item->columns()[index.column()];

    switch ( val.type() ) {
    case xviz::TabularData::Type::String:
        return QString::fromStdString(val.toString()) ;
    case xviz::TabularData::Type::Boolean:
        return val.toBoolean() ;
    case xviz::TabularData::Type::SignedInteger:
        return (qlonglong)val.toSignedInteger() ;
    case xviz::TabularData::Type::UnsignedInteger:
        return (qulonglong)val.toUnsignedInteger() ;
    case xviz::TabularData::Type::Float:
        return val.toFloat() ;
    case xviz::TabularData::Type::Double:
        return val.toDouble() ;

    }
}
//! [3]

//! [4]
Qt::ItemFlags TabularDataModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}
