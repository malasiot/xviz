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

class UIElement: public QObject {
public:
    virtual ~UIElement() {
        for( UIElement *child: children_ )
            delete child ;
    }
    virtual void config(const std::vector<xviz::Channel> &) {}
    virtual void updateState(const xviz::msg::StateUpdate &) {}

    virtual void buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) = 0 ;

    virtual void getChannels(QVector<QByteArray> &channels) const {}

    virtual QWidget *widget() const = 0 ;

    void addChild(UIElement *ele) {
        children_.push_back(ele) ;
    }

    const std::vector<UIElement *> getChildren() const { return children_ ; }


protected:
    static void setup_flex(const QDomElement &ele, QWidget *widget) ;
    static void parseChannels(const QDomElement &e, QVector<QByteArray> &channels) ;


     std::vector<UIElement *> children_ ;
};

class UIElementFactory {
public:

    UIElement *build(const QDomElement &e, QWidget *parent) const ;
};


class SpacerWidget : public QWidget {

  Q_OBJECT

  public:
    SpacerWidget(QWidget *parent = 0): QWidget(parent) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    QSize minimumSizeHint() const override { return QSize(0, 0) ;}
};

#endif
