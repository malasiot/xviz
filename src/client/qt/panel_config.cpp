#include "panel_config.hpp"

#include <QtCore/QDebug>
#include <QtCore/QJsonArray>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>

PanelConfig * PanelConfig::fromJSON(const QString &src) {

    QFile loadFile(src);

    if (!loadFile.open(QIODevice::ReadOnly)) {
       qWarning("Couldn't open config file.");
       return nullptr ;
    }

    QByteArray data = loadFile.readAll();

    QJsonDocument doc = QJsonDocument::fromJson(data) ;

    QJsonObject json = doc.object() ;

    return fromJSON(json) ;
}

PanelConfig * PanelConfig::fromJSON(const QJsonObject &json) {
    QString type ;
    if ( json.contains("type") && json["type"].isString() )
        type = json["type"].toString() ;
    else return nullptr ;

    PanelConfig *cfg = nullptr ;

    if ( type == "vertical_layout")
        cfg = new VerticalLayoutConfig() ;
    else if ( type == "horizontal_layout" )
        cfg = new HorizontalLayoutConfig() ;
    else if ( type == "image" )
        cfg = new ImagePanelConfig() ;
    else if ( type == "table" )
        cfg = new TablePanelConfig() ;
    else if ( type == "chart" )
        cfg = new ChartPanelConfig() ;
    else return nullptr ;

    if ( !cfg->parseJSON(json) ) return nullptr;

    return cfg ;
}

void PanelConfig::getChannelsRecursive(QVector<QByteArray> &channels) const {
    auto channels_this = getChannels() ;
    channels.append(channels_this) ;

     for( const auto &c: getChildren() ) {
         c->getChannelsRecursive(channels) ;
     }
}


bool PanelConfig::parseChildren(const QJsonObject &json) {
    if ( json.contains("children") && json["children"].isArray() ) {
        QJsonArray childrenArray = json["children"].toArray();

        for ( int i = 0; i < childrenArray.size(); ++i ) {
            QJsonValue val = childrenArray[i] ;
            if ( val.isObject() ) {
                QJsonObject obj = val.toObject() ;

                auto child = PanelConfig::fromJSON(obj);
                if ( child )
                    children_.push_back(std::move(child)) ;
            }
        }

        return true ;
    }

    return false ;
}

bool VerticalLayoutConfig::parseJSON(const QJsonObject &json) {
    return parseChildren(json) ;
}

bool HorizontalLayoutConfig::parseJSON(const QJsonObject &json) {
    return parseChildren(json) ;
}


bool ComponentConfig::parseJSON(const QJsonObject &json) {
    if (json.contains("title") && json["title"].isString())
        title_ = json["title"].toString() ;

    if (json.contains("description") && json["description"].isString())
        description_ = json["description"].toString() ;

    return true ;
}

bool ImagePanelConfig::parseJSON(const QJsonObject &json)
{
    ComponentConfig::parseJSON(json) ;

    if ( json.contains("channels") && json["channels"].isArray() ) {
        QJsonArray channelArray = json["channels"].toArray();

        for ( int i = 0; i < channelArray.size(); ++i ) {
            QJsonValue val = channelArray[i] ;
            if ( val.isString() )
                channels_.append(val.toString().toUtf8()) ;
        }
    }

    return !channels_.empty() ;
}

bool TablePanelConfig::parseJSON(const QJsonObject &json)
{
     ComponentConfig::parseJSON(json) ;

    if (json.contains("channel") && json["channel"].isString())
        channel_ = json["channel"].toString().toUtf8() ;

    return !channel_.isEmpty() ;

}

bool ChartPanelConfig::parseJSON(const QJsonObject &json)
{
    ComponentConfig::parseJSON(json) ;

    if ( json.contains("channels") && json["channels"].isArray() ) {
        QJsonArray channelArray = json["channels"].toArray();

        for ( int i = 0; i < channelArray.size(); ++i ) {
            QJsonValue val = channelArray[i] ;
            if ( val.isString() )
                channels_.append(val.toString().toUtf8()) ;
        }
    }

    return !channels_.empty() ;

}
