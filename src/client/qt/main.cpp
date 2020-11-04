#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include <QtCore/QFile>

#include "client.hpp"
#include "session_config.hpp"

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
    if ( parser.isSet(configOption) ) {
        QString configPath = parser.value("config") ;
        QFile loadFile(configPath);

        if (!loadFile.open(QIODevice::ReadOnly)) {
           qWarning("Couldn't open config file.");
        }

        QByteArray saveData = loadFile.readAll();

        QJsonDocument doc = QJsonDocument::fromJson(saveData) ;

        PanelConfig config ;
        config.fromJSON(doc.object()) ;

        qDebug() << "OK";
    }

    EchoClient client(QUrl(QStringLiteral("ws://localhost:9002/test")), debug);
    QObject::connect(&client, &EchoClient::closed, &a, &QCoreApplication::quit);

    return a.exec();
}
