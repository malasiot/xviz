#ifndef XVIZ_QT_UI_ELEMENT_FACTORY_HPP
#define XVIZ_QT_UI_ELEMENT_FACTORY_HPP

#include <xviz/channel.hpp>
#include <vector>

#include <QWidget>
#include <QDomElement>

namespace xviz {
namespace msg {
    class StateUpdate ;
}
}

class UIElementFactory ;

class UIElement {
public:
    virtual ~UIElement() {
        for( UIElement *child: children_ )
            delete child ;
    }
    virtual void config(const std::vector<xviz::Channel> &) {} ;
    virtual void updateState(const xviz::msg::StateUpdate &) {};

    virtual QWidget *buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) = 0 ;

    virtual void getChannels(QVector<QByteArray> &channels) const {}

    QWidget *widget() const { return widget_ ; }

    void addChild(UIElement *ele) {
        children_.push_back(ele) ;
    }

    const std::vector<UIElement *> getChildren() const { return children_ ; }

protected:
    QWidget *widget_ ;
    std::vector<UIElement *> children_ ;
};

class UIElementFactory {
public:

    UIElement *build(const QDomElement &e, QWidget *parent) const ;
};


#endif
