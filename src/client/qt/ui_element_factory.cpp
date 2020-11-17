#include "ui_element_factory.hpp"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

static void parse_container_children(const UIElementFactory &fac, const QDomElement &ele, UIElement *parent) {

    QDomElement node = ele.firstChildElement();

    while ( !node.isNull() ) {
        UIElement *child = fac.build(node, parent->widget()) ;
        if ( child )
            parent->addChild(child) ;

        node = node.nextSiblingElement() ;
    }
}

enum class Orientation { Horizontal, Vertical, Explicit } ;

class Box: public UIElement {
public:

    Box( Orientation orient = Orientation::Explicit): orient_(orient) {}

    QWidget *buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) override {
        widget_ = new QWidget(parent) ;
        widget_->setStyleSheet("border:1px solid rgb(0, 255, 0); ");

        QString orient = ele.attribute("orient", "horizontal") ;
        QString align = ele.attribute("align", "stretch") ;
        int flex = ele.attribute("flex", "0").toInt() ;

        QBoxLayout *layout ;

        Orientation o(orient_) ;
        if ( o == Orientation::Explicit ) {
            if ( orient == "vertical" )
                o = Orientation::Vertical ;
            else
                o = Orientation::Horizontal ;
        }

        switch ( o ) {
        case Orientation::Vertical:
            layout = new QVBoxLayout(widget_) ;
            break ;
        case Orientation::Horizontal:
            layout = new QHBoxLayout(widget_) ;
            break ;
        }

        widget_->setLayout(layout) ;

        parse_container_children(fac, ele, this) ;

        for ( UIElement *child: children_ ) {
            layout->addWidget(child->widget()) ;

            if ( o == Orientation::Vertical ) {
                if ( align == "start" )
                    layout->setAlignment(child->widget(), Qt::AlignLeft ) ;
                else if ( align == "center" )
                    layout->setAlignment(child->widget(), Qt::AlignHCenter ) ;
                else if ( align == "end" )
                    layout->setAlignment(child->widget(), Qt::AlignRight ) ;
            } else {
                if ( align == "start" )
                    layout->setAlignment(child->widget(), Qt::AlignTop ) ;
                else if ( align == "center" )
                    layout->setAlignment(child->widget(), Qt::AlignVCenter ) ;
                else if ( align == "end" )
                    layout->setAlignment(child->widget(), Qt::AlignBottom ) ;
            }
        }


        QSizePolicy pc(QSizePolicy::Preferred, QSizePolicy::Preferred) ;
        pc.setHorizontalStretch(flex) ;
        widget_->setSizePolicy(pc) ;

        return widget_ ;
    }

private:

    Orientation orient_ ;
};

class VBox: public Box {
public:
    VBox(): Box(Orientation::Vertical) {}

};

class HBox: public Box {
public:
    HBox(): Box(Orientation::Horizontal) {}
};

class Label: public UIElement {
public:
    QWidget *buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) override {
        QLabel *label = new QLabel(parent) ;
        label->setText(ele.attribute("value")) ;
        int flex = ele.attribute("flex", "0").toInt() ;

        QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        policy.setHorizontalStretch(flex) ;
        policy.setVerticalStretch(flex) ;
        label->setSizePolicy(policy) ;

        return widget_ = label ;
    }

};

class Button: public UIElement {
public:
    QWidget *buildWidget(const UIElementFactory &, const QDomElement &ele, QWidget *parent) override {
        QPushButton *button = new QPushButton(parent) ;
        button->setText(ele.attribute("label")) ;

        return widget_ = button ;
    }

};

UIElement *UIElementFactory::build(const QDomElement &e, QWidget *parent) const {

    UIElement *ele = nullptr ;

    QString name = e.tagName() ;
    if ( name == "vbox" )
        ele = new VBox() ;
    else if ( name == "hbox" )
        ele = new HBox() ;
    else if ( name == "label" )
        ele = new Label() ;
    else if ( name == "button" )
        ele = new Button() ;
    else if ( name == "window" )
        ele = new Box() ;

    ele->buildWidget(*this, e, parent) ;

    return ele ;
}

