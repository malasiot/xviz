#include "main_window.hpp"
#include "ui_element_factory.hpp"

#include <QHBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <QSettings>


MainWindow::MainWindow(const QString &config, QWidget *parent): QMainWindow(parent) {

    readSettings() ;

    QFile loadFile(config);

    if (!loadFile.open(QIODevice::ReadOnly)) {
       qWarning("Couldn't open config file.");
       return ;
    }

    QByteArray data = loadFile.readAll();

    QDomDocument doc ;
    doc.setContent(data) ;

    QDomElement root = doc.documentElement();

    UIElementFactory factory ;

    if ( root.tagName() == "window" ) {
        root_element_ = factory.build(root, parent) ;
        setCentralWidget(root_element_->widget()) ;
    }
}

MainWindow::~MainWindow()
{
    delete root_element_ ;
}

void MainWindow::writeSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    //optional check if the user really want to quit
    // and/or if the user want to save settings
    writeSettings();
    event->accept();
}

void MainWindow::getChannelsRecursive(QVector<QByteArray> &channels) {
    root_element_->getChannels(channels) ;
}


void MainWindow::updateState(const xviz::msg::StateUpdate &su) {
    root_element_->updateState(su) ;
}

