#ifndef XVIZ_QT_CHART_PANEL_HPP
#define XVIZ_QT_CHART_PANEL_HPP

#include "panel.hpp"
#include "panel_config.hpp"
#include "chart/chart_widget.hpp"
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVBoxLayout>
#include <QtCharts/QChartView>

class ChartPanel: public Panel {
    Q_OBJECT
public:
    ChartPanel(const ChartPanelConfig &config, QWidget *parent) ;

    void config(const std::vector<xviz::Channel> &channels) override ;
    void updateState(const xviz::msg::StateUpdate &) override ;
private:

    ChartPanelConfig config_ ;
    ChartWidget *widget_ ;
    QVBoxLayout *layout_ ;
};

#endif
