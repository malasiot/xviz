#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>

#include "client.hpp"
#include "panel_config.hpp"

using namespace xviz ;
using namespace std ;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Qt xviz client");
    parser.addHelpOption();

    QCommandLineOption dbgOption(QStringList() << "d" << "debug", "Debug output [default: off].");
    parser.addOption(dbgOption);
    QCommandLineOption configOption(QStringList() << "c" << "config", "Configuration file", "xx");
    parser.addOption(configOption) ;

    parser.process(a);

    bool debug = parser.isSet(dbgOption);

       QVector<QByteArray> channels ;
    if ( parser.isSet(configOption) ) {
        QString configPath = parser.value("config") ;
        QFile loadFile(configPath);

        if (!loadFile.open(QIODevice::ReadOnly)) {
           qWarning("Couldn't open config file.");
        }

        QByteArray saveData = loadFile.readAll();

        QJsonDocument doc = QJsonDocument::fromJson(saveData) ;

        auto config = PanelConfig::fromJSON(doc.object()) ;

        if ( config ) {
            config->getChannelsRecursive(channels) ;
        }

    }

    EchoClient client(QUrl(QStringLiteral("ws://localhost:9002/test")), channels, debug);
    QObject::connect(&client, &EchoClient::closed, &a, &QCoreApplication::quit);


    qDebug() << "OK";

     return a.exec();


}
