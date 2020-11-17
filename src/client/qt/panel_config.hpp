#ifndef PANEL_CONFIG_HPP
#define PANEL_CONFIG_HPP

#include <QDomElement>
#include <QVector>
#include <QByteArray>

#include <memory>
#include <vector>

class UIConfig {
public:

    static UIConfig * fromXMLFile(const QString &doc) ;

    virtual QVector<QByteArray> getChannels() const {
        return QVector<QByteArray>() ;
    }

    virtual const std::vector<UIConfig *> &getChildren() const {
        return children_ ;
    }

    virtual ~UIConfig() {
        for( UIConfig *child: children_ )
            delete child ;
    }

    void getChannelsRecursive(QVector<QByteArray> &channels) const ;

protected:

    static UIConfig *fromXML(const QDomElement &root) ;
    virtual bool parseXML(const QDomElement &obj) = 0 ;

    bool parseChildren(const QDomElement &obj) ;

    UIConfig() = default ;

protected:

    std::vector<UIConfig *> children_ ;
};

class ComponentConfig: public UIConfig {
protected:
    bool parseXML(const QDomElement &obj) ;

    QString title_, description_ ;
};

class VerticalLayoutConfig: public UIConfig {
protected:
    bool parseXML(const QDomElement &obj) override;
};

class HorizontalLayoutConfig: public UIConfig {
protected:
    bool parseXML(const QDomElement &obj) override;
};

class ImagePanelConfig: public ComponentConfig {
public:

     virtual QVector<QByteArray> getChannels() const override {
         return channels_ ;
     }


    QVector<QByteArray> channels_ ;
protected:

    bool parseXML(const QDomElement &obj) override;

};

class ChartPanelConfig: public ComponentConfig {
public:

     virtual QVector<QByteArray> getChannels() const override {
        return channels_ ;
     }


    QVector<QByteArray> channels_;
protected:

    bool parseXML(const QDomElement &obj) override;

};

class TablePanelConfig: public ComponentConfig {
public:

     virtual QVector<QByteArray> getChannels() const override {
         QVector<QByteArray> clist ;
         clist << channel_ ;
         return clist ;
     }

protected:

    bool parseXML(const QDomElement &obj) override;

    QByteArray channel_ ;
};



#endif // PANEL_CONFIG_HPP
