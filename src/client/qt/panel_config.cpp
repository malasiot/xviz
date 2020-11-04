#include "panel_config.hpp"

#include <QtCore/QJsonArray>

PanelConfig::PanelConfig()
{

}

bool PanelConfig::fromJSON(const QJsonObject &json)
{
    if (json.contains("name") && json["name"].isString())
        name_ = json["name"].toString() ;

    if (json.contains("description") && json["description"].isString())
        description_ = json["description"].toString() ;

    if ( json.contains("type") && json["type"].isString() )
        type_ = json["type"].toString() ;
    else return false ;

    if ( json.contains("channels") && json["channels"].isArray() ) {
        QJsonArray channelArray = json["channels"].toArray();
        channels_.reserve(channelArray.size()) ;

        for ( int i = 0; i < channelArray.size(); ++i ) {
            QJsonValue val = channelArray[i] ;
            if ( val.isString() )
                channels_.append(val.toString()) ;
        }
    }

    if ( json.contains("children") && json["children"].isArray() ) {
        QJsonArray childrenArray = json["children"].toArray();
        children_.reserve(childrenArray.size()) ;

        for ( int i = 0; i < childrenArray.size(); ++i ) {
            QJsonValue val = childrenArray[i] ;
            if ( val.isObject() ) {
                PanelConfig child ;
                QJsonObject obj = val.toObject() ;
                if ( child.fromJSON(obj) )
                    children_.push_back(std::move(child)) ;
            }
        }
    }

    return true ;

}
