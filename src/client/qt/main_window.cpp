#include "main_window.hpp"
#include "ui_element_factory.hpp"

#include <QHBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QLabel>
#include <QFile>

/*
QVector<QWidget *> parseContainerChildren(const QDomElement &ele, QWidget *parent);
QWidget *makeVBox(const QDomElement &ele, QWidget *parent) ;
QWidget *makeHBox(const QDomElement &ele, QWidget *parent) ;
QWidget *makeButton(const QDomElement &ele, QWidget *parent) ;
QWidget *makeLabel(const QDomElement &ele, QWidget *parent) ;
*/

MainWindow::MainWindow(const QString &config, QWidget *parent): QMainWindow(parent) {

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

void MainWindow::config(const std::vector<xviz::Channel> &channelInfo) {
    root_element_->config(channelInfo) ;
}

void MainWindow::updateState(const xviz::msg::StateUpdate &su) {
    root_element_->updateState(su) ;
}

