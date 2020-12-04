#include "chart_element.hpp"
#include "qt_graphics_helpers.hpp"
#include "chart/line_chart.hpp"
#include "chart/bar_chart.hpp"
#include "chart/raster_chart.hpp"

#include <xviz/chart.hpp>
#include <xviz/line_chart.hpp>
#include <xviz/bar_chart.hpp>
#include <xviz/raster_chart.hpp>

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


void ChartElement::updateState(const xviz::msg::StateUpdate &state_update)
{
    string channel_id = state_update.channel_id();
    //string object_id = state_update.object_id() ;
    if ( !channels_.contains(QByteArray::fromStdString(channel_id))) return ;
    string data = state_update.data() ;

    xviz::Chart *chart = xviz::Chart::decode(data) ;

    if ( !chart ) return ;

    Chart *qchart = nullptr;
    if ( xviz::LineChart *lc = dynamic_cast<xviz::LineChart *>(chart)) {
        qchart = new QLineChart(lc) ;
    } else if ( xviz::BarChart *lc = dynamic_cast<xviz::BarChart *>(chart)) {
        qchart = new QBarChart(lc) ;
    } else if ( xviz::RasterChart *lc = dynamic_cast<xviz::RasterChart *>(chart)) {
        qchart = new QRasterChart(lc) ;
    }

    if ( qchart ) {
        qchart->setAnnotations(chart->annotations());
        qchart->setTitle(QString::fromStdString(chart->title())) ;
        widget_->setChart(qchart) ;
    }

}
