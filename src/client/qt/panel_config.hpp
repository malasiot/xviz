#ifndef PANEL_CONFIG_HPP
#define PANEL_CONFIG_HPP

#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <memory>

class PanelConfig {
public:


    static PanelConfig * fromJSON(const QString &doc) ;

    virtual QVector<QByteArray> getChannels() const {
        return QVector<QByteArray>() ;
    }

    virtual const std::vector<PanelConfig *> &getChildren() const {
        return children_ ;
    }

    virtual ~PanelConfig() {
        for( PanelConfig *child: children_ )
            delete child ;
    }

    void getChannelsRecursive(QVector<QByteArray> &channels) const ;

protected:

    static PanelConfig *fromJSON(const QJsonObject &json) ;
    virtual bool parseJSON(const QJsonObject &obj) = 0 ;

    bool parseChildren(const QJsonObject &obj) ;

    PanelConfig() = default ;

protected:

    std::vector<PanelConfig *> children_ ;
};

class ComponentConfig: public PanelConfig {
protected:
    bool parseJSON(const QJsonObject &obj) ;

     QString title_, description_ ;
};

class VerticalLayoutConfig: public PanelConfig {
protected:
    bool parseJSON(const QJsonObject &obj) override;
};

class HorizontalLayoutConfig: public PanelConfig {
protected:
    bool parseJSON(const QJsonObject &obj) override;
};

class ImagePanelConfig: public ComponentConfig {
public:

     virtual QVector<QByteArray> getChannels() const override {
         return channels_ ;
     }


    QVector<QByteArray> channels_ ;
protected:

    bool parseJSON(const QJsonObject &obj) override;

};

class PlotPanelConfig: public ComponentConfig {
public:

     virtual QVector<QByteArray> getChannels() const override {
         QVector<QByteArray> clist ;
         clist << x_channel_ << y_channels_ ;
         return clist ;
     }

protected:

    bool parseJSON(const QJsonObject &obj) override;

    QByteArray x_channel_ ;
    QVector<QByteArray> y_channels_;
};

class TablePanelConfig: public ComponentConfig {
public:

     virtual QVector<QByteArray> getChannels() const override {
         QVector<QByteArray> clist ;
         clist << channel_ ;
         return clist ;
     }

protected:

    bool parseJSON(const QJsonObject &obj) override;

    QByteArray channel_ ;
};



#endif // PANEL_CONFIG_HPP
