#ifndef PANEL_CONFIG_HPP
#define PANEL_CONFIG_HPP

#include <QtCore/QJsonObject>
#include <QtCore/QStringList>
#include <QtCore/QVector>

class PanelConfig
{
public:
    PanelConfig();

    bool fromJSON(const QJsonObject &object) ;

    QString name_ ;
    QString description_ ;
    QStringList channels_ ;
    QString type_ ;

    std::vector<PanelConfig> children_ ;
};

#endif // PANEL_HPP
