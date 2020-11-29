#include <xviz/raster_chart.hpp>

#include "session.pb.h"

namespace xviz {


void RasterChart::write(xviz::msg::Chart &chart_data, const xviz::RasterChart *chart) {
    if ( chart == nullptr ) return;

    msg::RasterChart *msg_rc = new msg::RasterChart() ;
    chart_data.set_allocated_raster_chart(msg_rc) ;

    uint sz = chart->width() * chart->height() ;
    assert( chart->c().size() == sz ) ;

    for( uint i=0 ; i<sz ; i++ ) {
        msg_rc->add_c(chart->c()[i]) ;
    }

    const auto &X = chart->x() ;
    if ( !X.empty() ) {
        for ( uint i=0 ; i<X.size() ; i++ )
            msg_rc->add_x(X[i]) ;
    }

    const auto &Y = chart->y() ;
    if ( !Y.empty() ) {
        for ( uint i=0 ; i<Y.size() ; i++ )
            msg_rc->add_y(Y[i]) ;
    }

    msg_rc->set_width(chart->width()) ;
    msg_rc->set_height(chart->height()) ;
    msg_rc->set_colormap(chart->colorMap()) ;

    switch ( chart->shading() ) {
    case RasterChart::Flat:
        msg_rc->set_shading(msg::RasterChart_Shading_FLAT);
        break ;
    case RasterChart::Gouraud:
        msg_rc->set_shading(msg::RasterChart_Shading_GOURAUD);
        break ;
    }

    if ( chart->vrange_ ) {
        msg_rc->set_vmin(chart->vrange_->first) ;
        msg_rc->set_vmax(chart->vrange_->second) ;
    }

}

Chart *RasterChart::read(const msg::Chart &chart_data) {

    const msg::RasterChart &msg_rc = chart_data.raster_chart() ;

    uint w = msg_rc.width() ;
    uint h = msg_rc.height() ;
    uint sz = w * h ;

    assert( msg_rc.c_size() == sz ) ;

    std::vector<double> c, x, y ;

    c.resize(msg_rc.c_size()) ;
    x.resize(msg_rc.x_size()) ;
    y.resize(msg_rc.y_size()) ;

    for( uint i=0 ; i<msg_rc.c_size() ; i++ ) {
        c[i] = msg_rc.c(i) ;
    }

    for( uint i=0 ; i<msg_rc.x_size() ; i++ ) {
        x[i] = msg_rc.x(i) ;
    }

    for( uint i=0 ; i<msg_rc.y_size() ; i++ ) {
        y[i] = msg_rc.y(i) ;
    }

    RasterChart *rc = new RasterChart(w, h, c, x, y) ;

    switch ( msg_rc.shading() ) {
    case msg::RasterChart_Shading_FLAT:
        rc->setShading(RasterChart::Flat) ;
        break ;
    case msg::RasterChart_Shading_GOURAUD:
        rc->setShading(RasterChart::Gouraud) ;
        break ;
    }

    if ( msg_rc.has_vmin() && msg_rc.has_vmax() )
        rc->setValueRange(msg_rc.vmin(), msg_rc.vmax()) ;

    rc->setColormap(msg_rc.colormap()) ;

    return rc ;

}


}
