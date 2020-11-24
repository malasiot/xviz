#include <xviz/chart.hpp>
#include <xviz/line_chart.hpp>
#include <xviz/bar_chart.hpp>

using namespace std ;

#include "session.pb.h"

namespace xviz {

void Chart::setTicks(vector<Tick> &tics, const std::vector<double> &pos, const std::vector<string> &labels)
{
    assert ( labels.empty() | labels.size() == pos.size() ) ;
    bool has_labels = !labels.empty() ;

    for( uint i = 0 ; i<pos.size() ; i++ ) {
        Tick tic ;
        tic.pos_ = pos[i] ;
        if ( has_labels ) tic.label_ = labels[i] ;
        tics.push_back(std::move(tic)) ;
    }
}

void Chart::setTicksX(const std::vector<double> &pos, const std::vector<string> &labels) {
    setTicks(ticks_x_, pos, labels) ;
}

void Chart::setTicksY(const std::vector<double> &pos, const std::vector<string> &labels) {
    setTicks(ticks_y_, pos, labels) ;
}

string Chart::write(const Chart *c)
{
    msg::Chart chart_data ;

    chart_data.set_title(c->title()) ;
    chart_data.set_labelx(c->labelX()) ;
    chart_data.set_labely(c->labelY()) ;

    for ( const Tick &tic: c->ticks_x_ ) {
        chart_data.add_ticsx(tic.pos_);
        if ( !tic.label_.empty() ) chart_data.add_ticlabelsx(tic.label_) ;
    }

    for ( const Tick &tic: c->ticks_y_ ) {
        chart_data.add_ticsy(tic.pos_);
        if ( !tic.label_.empty() ) chart_data.add_ticlabelsy(tic.label_) ;
    }


    if ( const LineChart *lc = dynamic_cast<const LineChart *>(c) ) {
        LineChart::write(chart_data, lc);
    } else if ( const BarChart *bc = dynamic_cast<const BarChart *>(c) ) {
        BarChart::write(chart_data, bc);
    }

    for( const auto &a: c->annotations() ) {
        msg::Annotation *msg = Annotation::write(a) ;
        chart_data.mutable_annotations()->AddAllocated(msg);
    }

    return chart_data.SerializeAsString();
}

Chart *Chart::read(const string &bytes)
{
    msg::Chart chart_data ;
    if ( !chart_data.ParseFromString(bytes) ) return nullptr ;

    Chart *chart = nullptr ;
    if ( !chart_data.line_series().empty() ) {
        chart = LineChart::read(chart_data) ;
    } else if ( !chart_data.bar_series().empty() ) {
        chart = BarChart::read(chart_data) ;
    }

    if ( !chart ) return nullptr ;

    for( const auto &a: chart_data.annotations() ) {
        Annotation l = Annotation::read(a) ;
        chart->addAnnotation(l) ;
    }


    chart->setLabelX(chart_data.labelx()) ;
    chart->setLabelY(chart_data.labely()) ;
    chart->setTitle(chart_data.title()) ;

    bool has_tic_labels_x = chart_data.ticlabelsx_size() != 0 ;
    for( size_t i=0 ; i<chart_data.ticsx_size() ; i++ ) {
        Tick tic ;
        tic.pos_ = chart_data.ticsx(i) ;
        if ( has_tic_labels_x )
            tic.label_ = chart_data.ticlabelsx(i) ;
        chart->ticks_x_.push_back(std::move(tic)) ;
    }

    bool has_tic_labels_y = chart_data.ticlabelsy_size() != 0 ;
    for( size_t i=0 ; i<chart_data.ticsy_size() ; i++ ) {
        Tick tic ;
        tic.pos_ = chart_data.ticsy(i) ;
        if ( has_tic_labels_y )
            tic.label_ = chart_data.ticlabelsy(i) ;
        chart->ticks_y_.push_back(std::move(tic)) ;
    }


    return chart ;
}


}
