#ifndef XVIZ_QT_TABULAR_ELEMENT_HPP
#define XVIZ_QT_TABULAR_ELEMENT_HPP

#include "ui_element_factory.hpp"
#include <xviz/tabular.hpp>
#include <QTreeWidget>

class TabularDataModel ;
class TabularElement: public UIElement {
    Q_OBJECT
public:
    TabularElement() ;

    void buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) override ;

    void getChannels(QVector<QByteArray> &channels) const override {
        channels.append(channels_) ;
    }

    void updateState(const xviz::msg::StateUpdate &) override ;

    QWidget *widget() const override { return widget_ ; }

private:

    QTreeView *widget_ ;
    TabularDataModel *model_ ;

    QVector<QByteArray> channels_ ;
};


class TabularDataModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit TabularDataModel(xviz::Tabular *data, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:

    std::unique_ptr<xviz::Tabular> data_ ;
    uint num_columns_ = 2 ;
};

#endif
