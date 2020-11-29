#include "raster_chart.hpp"

#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

static void hsv2rgb(float h, QColor &rgb)
{
    int i ;
    float f, p, q, t, r, g, b ;

    if ( h == 0.0 ) return ;

    // h = 360.0-h ;

    h /= 60.0 ;

    i = (int)h ;
    f = h - i ;
    p = 0  ;
    q = 1-f ;
    t = f ;

    switch (i)
    {
        case 0:
            r = 1 ;
            g = t ;
            b = p ;
            break ;
        case 1:
            r = q ;
            g = 1 ;
            b = p ;
            break ;
        case 2:
            r = p ;
            g = 1 ;
            b = t ;
            break ;
        case 3:
            r = p ;
            g = q ;
            b = 1 ;
            break ;
        case 4:
            r = t ;
            g = p ;
            b = 1 ;
            break ;
        case 5:
            r = 1 ;
            g = p ;
            b = q ;
            break ;
    }

    rgb = qRgb((int)(255.0*r), (int)(255.0*g), (int)(255.0*b)) ;
}

QRasterChart::QRasterChart(const xviz::RasterChart *rc): Chart(rc) {

    const auto &c = rc->c() ;
    const auto *vrange = rc->valueRange() ;
    if ( vrange == nullptr ) {
        c_min_ = *std::min_element(c.begin(), c.end()) ;
        c_max_ = *std::max_element(c.begin(), c.end()) ;
    } else {
        c_min_ = vrange->first ;
        c_max_ = vrange->second ;
    }
}

QColor QRasterChart::mapColor(double v) {
    const xviz::RasterChart *rc = chart() ;

    float nv = (( v - c_min_ ) / (c_max_ - c_min_ )) ;

    auto clr = xviz::ColorMap::interpolate(nv, rc->colorMap()) ;
    return QColor(clr[0]*255, clr[1]*255, clr[2]*255) ;
}

void QRasterChart::paintChart(QPainter &p, const QSize &sz)
{
    const xviz::RasterChart *rc = chart() ;
/*
    QVector<qreal> x_grid = getXCoords() ;
    QVector<qreal> y_grid = getYCoords() ;

    for( uint i=0 ; i<y_grid.size()-1 ; i++ ) {
        for( uint j=0 ; j<x_grid.size()-1 ; j++ ) {
            qreal x0 = x_grid[j], x1 = x_grid[j+1] ;
            qreal y0 = y_grid[i], y1 = y_grid[i+1] ;
            QRectF rect(x0, y0, x1-x0, y1-y0) ;
            double val = rc->c()[i * rc->width() + j]  ;
            QColor clr = mapColor(val) ;
            p.fillRect(rect, QBrush(clr)) ;
        }
    }
*/

    QImage im = render(sz) ;
    im.save("/tmp/oo.png");
   p.drawImage(0, -sz.height(), im);

}

QRectF QRasterChart::getDataBounds() {
    const xviz::RasterChart *rc = chart() ;

    float x_min, x_max, y_min, y_max ;

    if ( rc->x().empty() ) {
        x_min = 0.f ; x_max = rc->width()  ;
    } else {
        x_min = rc->x().front() ; x_max = rc->x().back() ;
    }

    if ( rc->y().empty() ) {
        y_min = 0.f ; y_max = rc->height() ;
    } else {
        y_min = rc->y().front() ; y_max = rc->y().back() ;
    }

    return QRectF(x_min, y_min, x_max - x_min, y_max - y_min) ;
}

void QRasterChart::makeLegendEntries() {
}


void QRasterChart::bilinear(QImage &im, const QRectF &rect, const QColor &clr00, const QColor &clr01, const QColor &clr10, const QColor &clr11) {
    uint x0 = ceil(rect.left()) ;
    uint y0 = ceil(rect.top()) ;
    uint x1 = ceil(rect.right()) ;
    uint y1 = ceil(rect.bottom()) ;

    for( uint i=y0 ; i<y1 ; i++ ) {
        float v = (i - rect.top())/rect.height() ;
        for( uint j=x0 ; j<x1 ; j++ ) {
            float h = (j - rect.left())/rect.width() ;

            float r = (clr00.red() * ( 1 - h ) + clr01.red() * h) * (1 - v) +
                    v * ( clr10.red() * ( 1 - h) + clr11.red() * h ) ;
            float g = (clr00.green() * ( 1 - h ) + clr01.green() * h) * (1 - v) +
                    v * ( clr10.green() * ( 1 - h) + clr11.green() * h ) ;
            float b = (clr00.blue() * ( 1 - h ) + clr01.blue() * h) * (1 - v) +
                    v * ( clr10.blue() * ( 1 - h) + clr11.blue() * h ) ;

            im.setPixel(j, i, qRgb(r, g, b));
        }
    }
}

void QRasterChart::flat(QImage &im, const QRectF &rect, const QColor &clr) {
    uint x0 = ceil(rect.left()) ;
    uint y0 = ceil(rect.top()) ;
    uint x1 = ceil(rect.right()) ;
    uint y1 = ceil(rect.bottom()) ;

    for( uint i=y0 ; i<y1 ; i++ ) {
        for( uint j=x0 ; j<x1 ; j++ ) {
           im.setPixel(j, i, qRgb(clr.red(), clr.green(), clr.blue()));
        }
    }
}

QImage QRasterChart::render(const QSize &sz)
{
    const xviz::RasterChart *rc = chart() ;

    QImage im(sz, QImage::Format_RGB32) ;

    im.fill(qRgb(255, 255, 255));

    uint cols = rc->width(), rows = rc->height() ;

    QVector<qreal> x_grid = getXCoords() ;
    QVector<qreal> y_grid = getYCoords() ;

    for( uint i=0 ; i<y_grid.size()-1 ; i++ ) {
        for( uint j=0 ; j<x_grid.size()-1 ; j++ ) {
            qreal x0 = x_grid[j], x1 = x_grid[j+1] ;
            qreal y1 = y_grid[i], y0 = y_grid[i+1] ;
            QRectF rect(x0, y0 + sz.height(), x1-x0, y1-y0) ;
            if ( rc->shading() == xviz::RasterChart::Gouraud ) {
                uint ip1 = std::min(i+1, rows-1) ;
                uint jp1 = std::min(j+1, cols-1) ;
                QColor clr00 = mapColor(rc->c()[ip1 * cols + j])  ;
                QColor clr01 = mapColor(rc->c()[ip1 * cols + jp1])  ;
                QColor clr11 = mapColor(rc->c()[i * cols + jp1])  ;
                QColor clr10 = mapColor(rc->c()[i * cols + j])  ;

                bilinear(im, rect.normalized(), clr00, clr01, clr10, clr11) ;
            } else {
                QColor clr = mapColor(rc->c()[i * cols + j])  ;

                flat(im, rect.normalized(), clr) ;
            }

        }
    }

    return im ;
}



QVector<qreal> QRasterChart::getXCoords() {
    const xviz::RasterChart *rc = chart() ;

    QVector<qreal> x ;

    const auto &xv = rc->x() ;

    if ( xv.empty() ) {
        for( uint i=0 ; i<=rc->width() ; i++ )
            x.append(x_axis_.transform(i));
    } else {
        for( const double &v: xv )
            x.append(x_axis_.transform(v));
    }

    return x ;
}

QVector<qreal> QRasterChart::getYCoords() {
    const xviz::RasterChart *rc = chart() ;

    QVector<qreal> y ;

    const auto &yv = rc->y() ;

    if ( yv.empty() ) {
        for( uint i=0 ; i<=rc->height() ; i++ )
            y.append(y_axis_.transform(i));
    } else {
        for( const double &v: yv )
            y.append(y_axis_.transform(v));
    }

    return y ;
}

