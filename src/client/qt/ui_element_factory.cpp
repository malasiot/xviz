#include "ui_element_factory.hpp"
#include "image_element.hpp"
#include "chart_element.hpp"
#include "tabular_element.hpp"
#include "scene_element.hpp"

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



void UIElement::parseChannels(const QDomElement &e, QVector<QByteArray> &channels) {
    QDomElement node = e.firstChildElement("channel") ;

    while ( !node.isNull() ) {
        QByteArray a = node.attribute("id").toUtf8() ;
        if ( !a.isEmpty() )
            channels.append(a) ;
        node = node.nextSiblingElement("channel");
    }

}
void UIElement::setup_flex(const QDomElement &ele, QWidget *widget) {
    int flex = ele.attribute("flex", "0").toInt() ;

    QSizePolicy sz(QSizePolicy::Expanding, QSizePolicy::Expanding) ;
    sz.setHorizontalStretch(flex);
    sz.setVerticalStretch(flex);

    widget->setSizePolicy(sz) ;
}

class Spacer: public UIElement {
public:
    void buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) override {
       stretch_ =  ele.attribute("flex", "0").toInt() ;
    }

    int stretch() const { return stretch_ ; }

    QWidget *widget() const override { return nullptr ; }

private:
    int stretch_ ;
};

enum class Orientation { Horizontal, Vertical, Explicit } ;

class Box: public UIElement {
public:

    Box( Orientation orient = Orientation::Explicit): orient_(orient) {}

    void getChannels(QVector<QByteArray> &channels) const {
        for( UIElement *child: children_ )
            child->getChannels(channels);
    }

    void updateState(const xviz::msg::StateUpdate &su) {
        for( UIElement *child: children_ )
            child->updateState(su);
    }

    void buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) override {
        widget_ = new QWidget(parent) ;
      //  widget_->setStyleSheet("border:1px solid rgb(0, 255, 0); ");

        QString orient = ele.attribute("orient", "horizontal") ;
        QString align = ele.attribute("align", "stretch") ;
        QString pack = ele.attribute("pack", "none");

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

        int i = 0 ;
        for ( UIElement *child: children_ ) {
            QWidget *w = child->widget();

            if ( w == nullptr ) {
                if ( Spacer *sp = dynamic_cast<Spacer *>(child) ) {
                    layout->insertStretch(i++, sp->stretch());
                    continue ;
                }
            }
            layout->addWidget(child->widget()) ;

            Qt::Alignment alignment = 0 ;

            if ( o == Orientation::Vertical ) {
                if ( align == "start" )
                    alignment |= Qt::AlignLeft ;
                else if ( align == "center" )
                    alignment |= Qt::AlignHCenter ;
                else if ( align == "end" )
                    alignment |= Qt::AlignRight ;
             } else {
                if ( align == "start" )
                    alignment |= Qt::AlignTop ;
                else if ( align == "center" )
                    alignment != Qt::AlignVCenter  ;
                else if ( align == "end" )
                    alignment |= Qt::AlignBottom ;
            }

            if ( alignment != 0 )
                layout->setAlignment(child->widget(), alignment ) ;

            ++i ;
        }

        if ( pack == "end" )
            layout->insertStretch(0) ;
        else if ( pack == "center" ) {
            layout->addStretch(1) ;
            layout->insertStretch(0, 1) ;
        }
        else if ( pack == "start" )
            layout->addStretch() ;


        setup_flex(ele, widget_) ;
    }

    QWidget *widget() const override { return widget_ ; }

private:

    QWidget *widget_ ;
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
    void buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) override {
        label_ = new QLabel(parent) ;
        label_->setText(ele.attribute("value")) ;
        label_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        setup_flex(ele, label_) ;
    }

    QWidget *widget() const override { return label_; }

private:
    QLabel *label_ ;
};

class Button: public UIElement {
public:
    void buildWidget(const UIElementFactory &, const QDomElement &ele, QWidget *parent) override {
        button_ = new QPushButton(ele.attribute("label"), parent) ;
        setup_flex(ele, button_) ;
    }

    QWidget *widget() const { return button_ ; }

private:
    QPushButton *button_ ;
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
    else if ( name == "spacer" )
        ele = new Spacer() ;
    else if ( name == "image" )
        ele = new ImageElement() ;
    else if ( name == "chart" )
        ele = new ChartElement() ;
    else if ( name == "tabular" )
        ele = new TabularElement() ;
    else if ( name == "scene" )
        ele = new SceneElement() ;

    if ( ele )
        ele->buildWidget(*this, e, parent) ;

    return ele ;
}

