#include "main_window.hpp"
#include "chart_panel.hpp"

#include <QHBoxLayout>
#include <QSplitter>

MainWindow::MainWindow(PanelConfig *pc, QWidget *parent): QMainWindow(parent) {

    QWidget *cw = makeLayout(pc, this) ;
    /*
    ImagePanelConfig config ;
    config.channels_.append("/data/image") ;
    panel_ = new ImagePanel(config, this) ;
    panels_.append(panel_) ;
*/
    setCentralWidget(cw);
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

QWidget *MainWindow::makeLayout(PanelConfig *config, QWidget *parent) {
    if ( VerticalLayoutConfig *vl = dynamic_cast<VerticalLayoutConfig *>(config) ) {
        QSplitter *splitter = new QSplitter(parent) ;
        splitter->setOrientation(Qt::Vertical);
        for( PanelConfig *child: vl->getChildren() ) {
            splitter->addWidget(makeLayout(child, splitter)) ;
        }
        return splitter ;
    } else if ( HorizontalLayoutConfig *vl = dynamic_cast<HorizontalLayoutConfig *>(config) ) {
        QSplitter *splitter = new QSplitter(parent) ;
         splitter->setOrientation(Qt::Horizontal);
        for( PanelConfig *child: vl->getChildren() ) {
            splitter->addWidget(makeLayout(child, splitter)) ;
        }
        return splitter ;
    } else if ( ImagePanelConfig *ip = dynamic_cast<ImagePanelConfig *>(config) ) {
        ImagePanel *panel = new ImagePanel(*ip, parent) ;
        panels_.append(panel) ;
        return panel ;
    } else if ( ChartPanelConfig *cp = dynamic_cast<ChartPanelConfig *>(config) ) {
        ChartPanel *panel = new ChartPanel(*cp, parent) ;
        panels_.append(panel) ;
        return panel ;
    } else {
        MockPanel *panel = new MockPanel(parent) ;
        return panel ;
    }
}

