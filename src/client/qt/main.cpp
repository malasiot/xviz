#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>


#include "client.hpp"
//#include "panel_config.hpp"
#include "main_window.hpp"

#include <thread>
#include <iostream>

#include <xviz/server.hpp>
#include <xviz/image.hpp>
#include <xviz/bar_chart.hpp>
#include <xviz/tabular.hpp>
#include <xviz/path.hpp>
#include <xviz/scene/scene.hpp>

#include <csignal>

using namespace xviz ;
using namespace std ;

xviz::Server server("/home/malasiot/tmp") ;
string imageChannel("/data/image"), chartChannel("/data/chart"), tabularChannel("/data/tabular") ;

void onConnected() {

  //  server.sendImageUri(imageChannel, "https://qph.fs.quoracdn.net/main-qimg-7213b23a51c7d8b97a299eaa9fe69849");

    server.push(imageChannel, Image("http://localhost:9002/image.png"));
/*
    LineChart lc ;

    LineSeries ls ;
    ls.setTitle("chart 1") ;
    ls.setMarker(MarkerHandle(new SimpleShapeMarker(SimpleShapeMarker::Square, 8, PenHandle(new SolidPen(Color::black())), BrushHandle(new SolidBrush(Color::red())))));
    ls.x() = { 0.0, 1.0, 2.0, 3.0 } ;
    ls.y() = { 0.0, -2.0, 4.0, 3.0 } ;
    ls.parseParamString("r--");
    lc.addSeries(ls) ;
    lc.setLabelX("X\ntriangles") ;
    lc.setLabelY("Y\n(num)") ;

 //   lc.setTicksX({0, 1.5, 2, 3}, {"zero", "one", "two", "three"}) ;


    server.sendLineChart(chartChannel, lc) ;
*/

    BarChart bc ;

    BarSeries bs ;
    bs.setTitle("chart 1") ;
    bs.setBrush(Brush(LinearGradient(0.0, 0.0, 0.0, 1.0, {{0.0, Color::blue()}, {1.0, Color::green()}}))) ;
    bs.setWidth(0.3) ;
    bs.x() = { 0.0, 1.0, 2.0, 3.0 } ;
    bs.height() = { 3.0, 4.1, 1.0, 2.5 } ;
    bc.addSeries(bs) ;
    bc.setLabelX("X") ;
    bc.setLabelY("Y") ;

    Path p ;
    p.builder().addRect(0.5, 1.5, 1.2, 0.8)
            .addEllipse(1.6, 1.4, 0.5, 0.5) ;

    //DrawableHandle d(new ShapeDrawable(p));
   // d->setPen({Color::red(), 2.5});
   // d->setBrush(Brush(RadialGradient(0.5, 0.5, 0.7, 0.7, 1, {{0.0, Color::red()}, {1.0, Color::white()}})));

   // bc.addAnnotation(d);

    server.push(chartChannel, bc) ;

    Tabular t{ { {"name", "Key"}, {"value", "Value"} }} ;
    TabularNode *n = t.addRow({ "name", "" });
    t.addRow({ "value", "2.0" }, n);

    server.push(tabularChannel, t) ;

 //   Scene scene ;
 //   scene.load("/home/malasiot/Downloads/2CylinderEngine.glb");
 //   cout << "ok" << endl ;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("ITI");
    QCoreApplication::setApplicationName("xviz-qt-client");


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


    MainWindow win(configFile) ;

    QVector<QByteArray> channels ;

    win.getChannelsRecursive(channels) ;

    std::thread t([&] {server.run(9002);});

    std::this_thread::sleep_for(std::chrono::seconds(1));


    WebSocketClient client(QUrl(url), channels, debug);

    QObject::connect(&client, &WebSocketClient::closed, &a, &QCoreApplication::quit);
    QObject::connect(&client, &WebSocketClient::connected, &a, &onConnected);
    QObject::connect(&client, &WebSocketClient::stateUpdated, &win, &MainWindow::updateState);


    client.connect() ;

    signal(SIGTERM, [](int sig) { QCoreApplication::quit(); });
    signal(SIGABRT, [](int sig) { QCoreApplication::quit(); });
    signal(SIGINT, [](int sig) { QCoreApplication::quit(); });
    signal(SIGKILL, [](int sig){ QCoreApplication::quit(); });

    win.show() ;

    return a.exec();


}
