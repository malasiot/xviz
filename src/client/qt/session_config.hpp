#ifndef SESSIONCONFIG_HPP
#define SESSIONCONFIG_HPP

#include <QString>
#include <QJsonDocument>
#include <QVector>

#include <memory>

#include "panel_config.hpp"

class SessionConfig
{
public:
    SessionConfig();

    void fromJSON(const QJsonDocument &src) ;

};

#endif // SESSIONCONFIG_HPP
