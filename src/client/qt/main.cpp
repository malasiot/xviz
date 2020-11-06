#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>


#include "client.hpp"
#include "panel_config.hpp"
#include "main_window.hpp"

#include <thread>

#include <xviz/server.hpp>

#include <csignal>

using namespace xviz ;
using namespace std ;

xviz::Server server("/home/malasiot/tmp") ;
xviz::Channel *imageChannel ;

void onConnected() {

  //  server.sendImageUri(imageChannel, "https://qph.fs.quoracdn.net/main-qimg-7213b23a51c7d8b97a299eaa9fe69849");

    server.sendImageUri(imageChannel, "http://localhost:9002/image.png");

 //   std::this_thread::sleep_for(std::chrono::seconds(1));

//    server.sendImageUri(imageChannel, "http://image3.png");
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QCommandLineParser parser;
    parser.setApplicationDescription("Qt xviz client");
    parser.addHelpOption();

    QCommandLineOption dbgOption(QStringList() << "d" << "debug", "Debug output [default: off].");
    QCommandLineOption urlOption(QStringList() << "u" << "url", "Server url", "url", "ws://localhost:9002/");

    parser.addOption(dbgOption);
    parser.addOption(urlOption) ;
    parser.addPositionalArgument("config", "Configuration file.");
    parser.process(a);

    bool debug = parser.isSet(dbgOption);

    const QStringList args = parser.positionalArguments();

    if ( args.size() == 0 ) {
        parser.showHelp() ;
        return 1 ;
    }

    QString configFile = args.at(0) ;
    QString url = parser.value("url") ;

    auto config = PanelConfig::fromJSON(configFile) ;



    MainWindow win(config) ;

    QVector<QByteArray> channels ;

    if ( config ) {
            config->getChannelsRecursive(channels) ;
    }

    imageChannel = server.createChannel("/data/image", xviz::Channel::IMAGE) ;
    server.createChannel("/data/table", xviz::Channel::TENSOR) ;
    server.createChannel("/data/plot_x", xviz::Channel::TENSOR) ;

    std::thread t([&] {server.run(9002);});

    std::this_thread::sleep_for(std::chrono::seconds(1));


    WebSocketClient client(QUrl(url), channels, debug);

    QObject::connect(&client, &WebSocketClient::closed, &a, &QCoreApplication::quit);
    QObject::connect(&client, &WebSocketClient::connected, &a, &onConnected);
    QObject::connect(&client, &WebSocketClient::connected, &win, &MainWindow::config);
    QObject::connect(&client, &WebSocketClient::stateUpdated, &win, &MainWindow::updateState);


    client.connect() ;

    signal(SIGTERM, [](int sig) { QCoreApplication::quit(); });
    signal(SIGABRT, [](int sig) { QCoreApplication::quit(); });
    signal(SIGINT, [](int sig) { QCoreApplication::quit(); });
    signal(SIGKILL, [](int sig){ QCoreApplication::quit(); });

    win.show() ;

    return a.exec();


}
