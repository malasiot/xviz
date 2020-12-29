#include <xviz/bar_chart.hpp>

using namespace std ;

#include "session.pb.h"

namespace xviz {

void BarChart::write(msg::Chart &chart_data, const BarChart *c)  {

    msg::BarChart *msg_bc = new msg::BarChart() ;
    chart_data.set_allocated_bar_chart(msg_bc) ;

    for( const BarSeries &ls : c->series() ) {
        msg::BarSeries *msg_ls = msg_bc->add_bar_series() ;

        msg_ls->set_width(ls.width());

        msg_ls->set_title(ls.title()) ;
        msg_ls->set_allocated_brush(Brush::write(ls.brush())) ;
        msg_ls->set_allocated_pen(Pen::write(ls.pen()));

        const auto &x = ls.x() ;
        const auto &y = ls.height() ;
        const auto &y0 = ls.bottom() ;

        for ( uint i=0 ; i<x.size() ; i++ ) {
            msg_ls->add_x(x[i]) ;
            msg_ls->add_y(y[i]) ;
            if ( !y0.empty() ) msg_ls->add_y0(y0[i]) ;
        }
    }


}

Chart *BarChart::read(const msg::Chart &chart_data) {

    BarChart *lc = new BarChart ;

    const msg::BarChart &msg_bc = chart_data.bar_chart() ;

    for( const msg::BarSeries &msg_ls: msg_bc.bar_series() ) {
        BarSeries ls ;

        ls.setTitle(msg_ls.title()) ;
        ls.setWidth(msg_ls.width()) ;

        for( double x: msg_ls.x() ) ls.x().push_back(x) ;
        for( double y: msg_ls.y() ) ls.height().push_back(y) ;
        for( double y0: msg_ls.y0() ) ls.bottom().push_back(y0) ;

        if ( msg_ls.has_pen() ) ls.setPen(Pen::read(msg_ls.pen()));
        if ( msg_ls.has_brush() ) ls.setBrush(Brush::read(msg_ls.brush()));

        lc->addSeries(ls) ;
    }

    return lc ;
}



}
