#include <xviz/line_chart.hpp>

using namespace std ;

#include "session.pb.h"

namespace xviz {

void LineSeries::parseParamString(const char *src) {

    const char *c = src ;
    int markerChar = -1 ;
    int dash = -1  ;

    if ( c == nullptr ) return ;

    Color clr = Color::black() ;

    while (*c != 0 ) {
        switch (*c) {
        case 'y': clr = Color::yellow(); break ; //yellow
        case 'm': clr = Color::magenta() ; break ; // magenta
        case 'c': clr = Color::cyan() ; break ; // cyan
        case 'r': clr = Color::red() ; break ; // red
        case 'g': clr = Color::green() ; break ; // green
        case 'b': clr = Color::blue() ; break ; // blue
        case 'w': clr = Color::white() ; break ;// white
        case 'k': clr = Color::black() ; break ;// black
        case '.': case 'o': case 'x': case '+': case '*':
        case 's': case 'd': case 'v': case '^': case '<':
        case '>': markerChar = *c ; break ;
        case '-': if ( *(c+1) == '-' ) { dash = 2 ; c++ ; } // dashed
            else if ( *(c+1) == '.') { dash = 3 ; c++ ; }// dash-dot
            else { dash = 0 ; } // solid
            break ;
        case ':': dash = 1 ; break ; // dotted
        default: break ;
        }

        c++ ;
    }

    bool has_marker = true ;
    StockMarkerShape shape ;
    switch ( markerChar ) {

    case 'o':  shape = StockMarkerShape::Circle ; break ;
    case 'x':  shape = StockMarkerShape::XMark ; break ;
    case '+':  shape = StockMarkerShape::Plus ; break ;
    case 's':  shape = StockMarkerShape::Square ; break ;
    case '*':  shape = StockMarkerShape::Star ; break ;
    case 'd':  shape = StockMarkerShape::Diamond ; break ;
    case 'v':  shape = StockMarkerShape::TriangleDown ; break ;
    case '^':  shape = StockMarkerShape::TriangleUp ; break ;
    case '<':  shape = StockMarkerShape::TriangleLeft ; break ;
    case '>':  shape = StockMarkerShape::TriangleRight ; break ;
    case '.':  shape = StockMarkerShape::Point ; break ;
    default:   has_marker = false ;
    }

    if ( has_marker )
        setMarker(Marker(shape, Pen(), Brush(Color::white()), 6.0)) ;

    Pen pen(clr) ;

    switch ( dash )
    {
    case 0:
        pen.setLineStyle(SolidLine) ;
        break ;
    case 1:
        pen.setLineStyle(DotLine) ;
        break ;
    case 2:
        pen.setLineStyle(DashLine) ;
        break ;
    case 3:
        pen.setLineStyle(DashDotLine) ;
        break ;

    }

     setPen(pen);
}



void LineChart::write(msg::Chart &chart_data, const LineChart *c)  {
    if ( c == nullptr ) return;

    for( const LineSeries &ls : c->series() ) {
        msg::LineSeries *msg_ls = chart_data.add_line_series() ;

        msg_ls->set_title(ls.title()) ;
        msg_ls->set_allocated_brush(Brush::write(ls.brush())) ;
        msg_ls->set_allocated_pen(Pen::write(ls.pen()));
        msg_ls->set_allocated_marker(Marker::write(ls.marker()));

        const auto &x = ls.x() ;
        const auto &y = ls.y() ;
        const auto &e = ls.e() ;

        for ( uint i=0 ; i<x.size() ; i++ ) {
            msg_ls->add_x(x[i]) ;
            msg_ls->add_y(y[i]) ;
            if ( !e.empty() ) msg_ls->add_e(e[i]) ;
        }
    }

}

Chart *LineChart::read(const msg::Chart &chart_data) {

    LineChart *lc = new LineChart ;

    for( const msg::LineSeries &msg_ls: chart_data.line_series() ) {
        LineSeries ls ;

        ls.setTitle(msg_ls.title()) ;
        for( double x: msg_ls.x() ) ls.x().push_back(x) ;
        for( double y: msg_ls.y() ) ls.y().push_back(y) ;
        for( double e: msg_ls.e() ) ls.e().push_back(e) ;

        if ( msg_ls.has_pen() ) ls.setPen(Pen::read(msg_ls.pen()));
        if ( msg_ls.has_brush() ) ls.setBrush(Brush::read(msg_ls.brush()));
        if ( msg_ls.has_marker() ) ls.setMarker(Marker::read(msg_ls.marker()));
        lc->addSeries(ls) ;
    }

    return lc ;
}



}
