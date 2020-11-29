#include "annotation.hpp"
#include "qt_graphics_helpers.hpp"

void AnnotationPainter::paint(QPainter &c, const xviz::Matrix2d &m) {
    c.save() ;

    for( const xviz::Annotation &a: annotations_ ) {
        if ( a.type() == xviz::Annotation::Label ) {
            paintLabels(c,  m, static_cast<const xviz::LabelAnnotation &>(a)) ;
        } else if ( a.type() == xviz::Annotation::Shape ) {
            paintShapes(c,  m, static_cast<const xviz::ShapeAnnotation &>(a)) ;
        } else if ( a.type() == xviz::Annotation::Marker ) {
            paintMarkers(c,  m, static_cast<const xviz::MarkerAnnotation &>(a)) ;
        }
    }

    c.restore() ;
}


void AnnotationPainter::paintLabels(QPainter &c, const xviz::Matrix2d &m, const xviz::LabelAnnotation &la)
{
    const auto &labels = la.labels() ;
    const auto &positions = la.positions() ;

    int flags = la.alignFlags() ;
    qreal offset_x = la.offset().x() ;
    qreal offset_y = la.offset().y() ;

    c.save() ;
    c.setPen(qPenFromSolidPen(la.pen())) ;
    c.setBrush(qBrushFromSolidBrush(la.brush())) ;

    if ( la.font() )
        c.setFont(qFontFromFont(*la.font())) ;

    QFontMetrics fm(c.font()) ;

    for( int i=0 ; i<labels.size() ; i++ ) {
        xviz::Vector2d pt = m.transform(positions[i]) ;
        qreal px = pt.x() ;
        qreal py = pt.y() ;
        QString str = QString::fromStdString(labels[i]) ;
        QRect br = fm.tightBoundingRect(str) ;

        if ( flags & xviz::LabelAnnotation::TextAlignLeft )
            px -= br.width() + offset_x ;
        else if ( flags & xviz::LabelAnnotation::TextAlignRight )
            px += offset_x ;
        else if ( flags & xviz::LabelAnnotation::TextAlignHCenter )
            px -= br.width()/2 ;

        if ( flags & xviz::LabelAnnotation::TextAlignTop )
            py -= offset_y ;
        else if ( flags & xviz::LabelAnnotation::TextAlignBottom )
            py += br.height() + offset_y ;
        else if ( flags & xviz::LabelAnnotation::TextAlignVCenter )
            py += br.height()/2 ;

        c.drawText(QPointF(px, py), QString::fromStdString(labels[i])) ;
    }
    c.restore() ;
}

void AnnotationPainter::paintShapes(QPainter &c, const xviz::Matrix2d &m, const xviz::ShapeAnnotation &sa)
{
    const auto &shapes = sa.shapes() ;

    c.save() ;
    c.setPen(qPenFromSolidPen(sa.pen())) ;
    c.setBrush(qBrushFromSolidBrush(sa.brush())) ;

    for( int i=0 ; i<shapes.size() ; i++ ) {
        QPainterPath p = qPathFromPath(shapes[i].transformed(m)) ;
        c.drawPath(p) ;
    }
    c.restore() ;
}

void AnnotationPainter::paintMarkers(QPainter &c, const xviz::Matrix2d &m, const xviz::MarkerAnnotation &ma)
{
    const auto &marker = ma.marker() ;
    const auto &positions = ma.positions() ;

    c.save() ;
    c.setPen(qPenFromSolidPen(ma.pen())) ;
    c.setBrush(qBrushFromSolidBrush(ma.brush())) ;

    QPainterPath p = qPathFromPath(marker) ;

    for( int i=0 ; i<positions.size() ; i++ ) {
        c.save() ;
        xviz::Vector2d pt = m.transform(positions[i]) ;
        qreal px = pt.x() ;
        qreal py = pt.y() ;
        c.translate(px, py);
        c.drawPath(p) ;
        c.restore() ;
    }

    c.restore() ;
}
