#include <xviz/qt/chart/color_ramp.hpp>

#include <QFontMetrics>

QRect ColorRamp::layout(float h) {

    float height = h * fraction_ ;

    axis_.computeLayout(height) ;

    qreal ox = axis_.size() ;

    axis_.computeTransform(height) ;

    float width = pad_ + width_ + ox ;

    return QRect(0, 0, width, height) ;
}

void ColorRamp::paint(QPainter &p, const QRect &box)
{
    float height = box.height() * fraction_ ;

    QRect bar_rect(pad_, -(box.height()/2-height/2), width_, -height) ;

    QImage im = renderBar(QSize(width_, height)) ;

    p.drawImage(bar_rect.normalized(), im.mirrored()) ;

    p.save() ;
    p.translate(pad_ + width_, 0) ;
    axis_.draw(p, box.width(), height) ;
    p.restore() ;

}

QImage ColorRamp::renderBar(const QSize &sz)
{
    QImage im(sz, QImage::Format_RGB32) ;

    for( int i=0 ; i<sz.height(); i++ ) {
        float v = ((float)i)/(sz.height()-1) ;
        auto clr = xviz::ColorMap::interpolate(v, cmap_) ;
        QRgb rgb = qRgb(clr[0]*255, clr[1]*255, clr[2]*255) ;

        for( int j=0 ; j<sz.width() ; j++ ) {
            im.setPixel(j, i, rgb) ;
        }

    }

    im.save("/tmp/oo.png");

    return im ;
}
