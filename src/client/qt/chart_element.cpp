#include "chart_element.hpp"
#include "qt_graphics_helpers.hpp"
#include "chart/line_chart.hpp"
#include "chart/bar_chart.hpp"

#include <xviz/chart.hpp>
#include <xviz/line_chart.hpp>
#include <xviz/bar_chart.hpp>

#include "session.pb.h"

using namespace std ;

ChartElement::ChartElement() {

}

void ChartElement::buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent)
{
    widget_ = new ChartWidget(parent) ;
    widget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    parseChannels(ele, channels_) ;
}

void ChartElement::config(const std::vector<xviz::Channel> &channels)
{

}

void ChartElement::updateState(const xviz::msg::StateUpdate &state_update)
{
    string channel_id = state_update.channel_id();
    //string object_id = state_update.object_id() ;
    if ( !channels_.contains(QByteArray::fromStdString(channel_id))) return ;
    string data = state_update.data() ;

    auto chart = xviz::Chart::read(data) ;
    if ( !chart ) return ;

    if ( xviz::LineChart *lc = dynamic_cast<xviz::LineChart *>(chart)) {
        QLineChart *line_chart = new QLineChart(lc) ;
        line_chart->setAnnotations(lc->annotations());
        line_chart->setTitle(QString::fromStdString(lc->title())) ;
        widget_->setChart(line_chart) ;
    } else if ( xviz::BarChart *lc = dynamic_cast<xviz::BarChart *>(chart)) {
        QBarChart *bar_chart = new QBarChart(lc) ;
        bar_chart->setAnnotations(lc->annotations());
        bar_chart->setTitle(QString::fromStdString(lc->title())) ;
        widget_->setChart(bar_chart) ;
    }

}
