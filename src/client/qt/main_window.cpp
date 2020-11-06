#include "main_window.hpp"
#include <QHBoxLayout>

MainWindow::MainWindow(PanelConfig *pc, QWidget *parent): QMainWindow(parent) {
    ImagePanelConfig config ;
    config.channels_.append("/data/image") ;
    panel_ = new ImagePanel(config, this) ;
    panels_.append(panel_) ;

    setCentralWidget(panel_);
}

void MainWindow::config(const std::vector<xviz::Channel> &channelInfo) {
    for( Panel *panel: panels_ ) {
        panel->config(channelInfo) ;
    }
}

void MainWindow::updateState(const xviz::msg::StateUpdate &su) {
    for( Panel *panel: panels_ ) {
        panel->updateState(su) ;
    }
}

