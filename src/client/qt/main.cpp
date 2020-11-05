#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>

#include "client.hpp"
#include "panel_config.hpp"

#include <thread>

#include <xviz/server.hpp>

#include <csignal>

using namespace xviz ;
using namespace std ;

xviz::Server server ;
xviz::Channel *imageChannel ;

void onConnected() {

    server.sendImageUri(imageChannel, "http://image2.png");

    std::this_thread::sleep_for(std::chrono::seconds(1));

    server.sendImageUri(imageChannel, "http://image3.png");
}

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


    imageChannel = server.createChannel("/data/image", xviz::Channel::IMAGE) ;
    server.createChannel("/data/table", xviz::Channel::TENSOR) ;
    server.createChannel("/data/plot_x", xviz::Channel::TENSOR) ;

    std::thread t([&] {server.run(9002);});

     std::this_thread::sleep_for(std::chrono::seconds(1));

    Client client(QUrl(QStringLiteral("ws://localhost:9002/test")), channels, debug);
    QObject::connect(&client, &Client::closed, &a, &QCoreApplication::quit);
    QObject::connect(&client, &Client::connected, &a, &onConnected);


    signal(SIGTERM, [](int sig) { QCoreApplication::quit(); });
        signal(SIGABRT, [](int sig) { QCoreApplication::quit(); });
        signal(SIGINT, [](int sig) { QCoreApplication::quit(); });
        signal(SIGKILL, [](int sig){ QCoreApplication::quit(); });

    qDebug() << "OK";

     return a.exec();


}
