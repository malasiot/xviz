#ifndef PANEL_CONFIG_HPP
#define PANEL_CONFIG_HPP

#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <memory>

namespace xviz {

class PanelConfig {
public:

    static std::unique_ptr<PanelConfig> fromJSON(const QJsonObject &doc) ;

    virtual QVector<QByteArray> getChannels() const {
        return QVector<QByteArray>() ;
    }

    virtual const std::vector<std::unique_ptr<PanelConfig>> &getChildren() const {
        return children_ ;
    }

    virtual ~PanelConfig() = default ;

    void getChannelsRecursive(QVector<QByteArray> &channels) const ;

protected:

    virtual bool parseJSON(const QJsonObject &obj) = 0 ;

    bool parseChildren(const QJsonObject &obj) ;

    PanelConfig() = default ;

protected:

    std::vector<std::unique_ptr<PanelConfig>> children_ ;
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

protected:

    bool parseJSON(const QJsonObject &obj) override;


     QVector<QByteArray> channels_ ;
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


}

#endif // PANEL_CONFIG_HPP
