#ifndef XVIZ_QT_MAIN_WINDOW_HPP
#define XVIZ_QT_MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QDomElement>
#include <QBoxLayout>
#include <QCloseEvent>

#include <xviz/channel.hpp>
#include "panel.hpp"

namespace xviz {
namespace msg {
    class StateUpdate ;
}
}

class UIElement ;

class MainWindow: public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(const QString &configFile, QWidget *parent = nullptr);
    ~MainWindow() ;

    void getChannelsRecursive(QVector<QByteArray> &channels);

public slots:
    void config(const std::vector<xviz::Channel> &channelInfo) ;
    void updateState(const xviz::msg::StateUpdate &state);

private:

    QWidget *makeLayout(const QDomElement &r, QWidget *parent) ;

private:
    UIElement *root_element_ ;

    void readSettings();
    void writeSettings();
    void closeEvent(QCloseEvent *event);
};



#endif
