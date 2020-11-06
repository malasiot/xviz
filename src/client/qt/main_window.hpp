#ifndef XVIZ_QT_MAIN_WINDOW_HPP
#define XVIZ_QT_MAIN_WINDOW_HPP

#include <QMainWindow>

#include "image_panel.hpp"

class MainWindow: public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(PanelConfig *config, QWidget *parent = nullptr);

public slots:
    void config(const std::vector<xviz::Channel> &channelInfo) ;
    void updateState(const xviz::msg::StateUpdate &state);

private:
    ImagePanel *panel_ ;
    QVector<Panel *> panels_ ;
};



#endif
