#include "image_rb.hpp"

#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsView>

///////////////////////////////////////////////////////////////////////
const int HRECT_SIZE = 7 ;
const int HRECT_SIZE_SMALL = 5 ;

GrabHandle::GrabHandle(QGraphicsItem *p): QGraphicsItem(p)
{
    setZValue(2);
    highlighted = false ;
    isSmall = false ;
    setCursor(QCursor(Qt::CrossCursor)) ;

}


void GrabHandle::setPos(const QPointF &p)
{
    prepareGeometryChange();
    pos = p ;
}

void GrabHandle::setHighlighted(bool h)
{
    if ( h != highlighted ) {
        highlighted = h ;
        update() ;
    }
}


void GrabHandle::setSmall(bool h)
{
    if ( h != isSmall ) {
        isSmall = h ;
        update() ;
    }
}

QRectF GrabHandle::boundingRect() const
{
    return sceneRect() ; //.adjusted(-20, -20, 20, 20) ;
}

QPainterPath GrabHandle::shape() const
{
    QPainterPath path;
    path.addRect(sceneRect());
    return path;
}

QRectF GrabHandle::sceneRect() const
{

    if ( scene()->views().isEmpty() ) return QRectF() ;

    int side = (isSmall) ? HRECT_SIZE_SMALL : HRECT_SIZE ;

    QSizeF sz = scene()->views()[0]->mapToScene(QRect(0, 0, side, side)).boundingRect().size() ;
    qreal sw = sz.width(), sh = sz.height();

    return QRectF(pos.x() - sw/2, pos.y() - sh/2, sw, sh) ;
}

void GrabHandle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setPen(Qt::SolidLine);
    painter->setBackgroundMode(Qt::TransparentMode) ;

    if ( highlighted )
    {
        painter->setBrush(QBrush(QColor(0, 0, 125, 100), Qt::SolidPattern)) ;

    }
    else
    {
        painter->setBrush(QBrush(QColor(255, 0, 125, 100), Qt::SolidPattern)) ;
    }

    //painter->drawRect(sceneRect());
    painter->drawEllipse(sceneRect()) ;
}

////////////////////////////////////////////////////////////////////////////////////////////////

RectRBand::RectRBand(QGraphicsItem *parent, QGraphicsScene *sc): QGraphicsItemGroup(parent)
{
    setZValue(1);

    sc->addItem(this) ;
    int i ;
    for( i=0 ; i<8 ; i++ ) {
        handle[i] = new GrabHandle(this) ;
        handle[i]->setZValue(2) ;
        addToGroup(handle[i]) ;
    }

    pen_.setStyle(Qt::SolidLine) ;
    pen_.setColor(QColor(255, 0, 0, 255)) ;

    brush_.setStyle(Qt::SolidPattern) ;
    brush_.setColor(QColor(0, 0, 125, 100)) ;
}

RectRBand::~RectRBand()
{

}


void RectRBand::setRect(const QRectF &r)
{
    prepareGeometryChange();
    rect = r ;

    qreal mx = (rect.left() + rect.right())/2 ;
    qreal my = (rect.top() + rect.bottom())/2 ;

    handle[0]->setPos(rect.topLeft()) ;
    handle[1]->setPos(QPointF(mx, rect.top())) ;
    handle[2]->setPos(rect.topRight()) ;
    handle[3]->setPos(QPointF(rect.right(), my)) ;
    handle[4]->setPos(rect.bottomRight()) ;
    handle[5]->setPos(QPointF(mx, rect.bottom())) ;
    handle[6]->setPos(rect.bottomLeft()) ;
    handle[7]->setPos(QPointF(rect.left(), my)) ;

    qreal pw = HRECT_SIZE;
    brect = shape().controlPointRect().adjusted(-pw/2, -pw/2, pw, pw);

    update() ;
}

QRectF RectRBand::boundingRect() const
{
    return brect ;
}

QPainterPath RectRBand::shape() const
{
    QPainterPath path;
    path.addRect(rect);
    return path;
}

void RectRBand::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{

    painter->setBrush(brush_) ;
    painter->setBackgroundMode(Qt::TransparentMode) ;

    painter->setPen(pen_);
    painter->drawRect(rect.normalized());
}

int RectRBand::whereIsPoint(const QPointF &p)
{
    QGraphicsItem *pItem = scene()->itemAt(p, QTransform()) ;

    int idx = -1 ;
    for( int i=0 ; i<8 ; i++ )
    {
        if ( pItem == handle[i] )
        {
            idx = i ;
            break ;
        }
    }

    if ( idx == -1 && pItem == this ) return 8 ;
    else return idx ;
}

GrabHandle *RectRBand::getHandle(int i)
{
    return handle[i] ;
}

void RectRBand::hideAll()
{
    for(int i=0 ; i<8 ; i++ )
        handle[i]->hide() ;
    hide() ;
}

void RectRBand::showAll()
{
    for(int i=0 ; i<8 ; i++ )
        handle[i]->show() ;
    show() ;

}

///////////////////////////////////////////////////////////////////////////



PolyRBand::PolyRBand(QGraphicsItem *parent):
    QGraphicsItem(parent)
{
    setZValue(1);

    pen_.setStyle(Qt::NoPen) ;
    drag_pen_.setStyle(Qt::SolidLine);
    drag_pen_.setColor(Qt::red) ;

}

void PolyRBand::setPolygon(const QPolygonF &ptlist)
{
    for(int i=0 ; i<handles.size() ; i++ ) {
        delete handles[i] ;
    }
    poly.clear() ;
    handles.clear() ;

    unsigned n = ptlist.size() ;
    for(int i=0 ; i<n ; i++ ) {
        poly.push_back(ptlist[i]) ;
        GrabHandle *pHandle = new GrabHandle( this ) ;
        pHandle->setPos(poly[i]) ;
        handles.push_back(pHandle) ;

    }

    updatePoly() ;
}

void PolyRBand::setPen(const QPen &pen) {
    pen_ = pen ;
    update() ;
}

void PolyRBand::setDragPen(const QPen &pen) {
    drag_pen_ = pen ;
    update() ;
}

void PolyRBand::setBrush(const QBrush &brush) {
    brush_ = brush ;
    update() ;
}


void PolyRBand::updatePoly()
{
    prepareGeometryChange();

    int i ;
    for( i=0 ; i<poly.size() ; i++ )
        handles[i]->setPos(poly[i]) ;

    qreal pw = 1.0;

    brect = shape().controlPointRect().adjusted(-pw/2-40, -pw/2-40, pw+40, pw+40);

    update() ;
}

QPolygonF PolyRBand::getPolygon() const
{
    return poly ;
}


QRectF PolyRBand::boundingRect() const
{
    return brect ;
}

QPainterPath PolyRBand::shape() const
{
    QPainterPath path;
    path.addPolygon(poly);
    return path;
}

void PolyRBand::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
/*
    painter->setBrush(brush_) ;
    painter->setBackgroundMode(Qt::TransparentMode) ;

    painter->setPen(pen_);

    if ( isClosed )
       painter->drawPolygon(poly) ;
    else
       painter->drawPolyline(poly);

    if ( drawText )
    {
        for( int i=0 ; i<poly.size() ; i++ )
        {
            QString text = labels_[i] ;

            QPointF pp = painter->transform().map(poly[i])  + QPointF(6, 10) ;

            painter->save() ;
            painter->resetTransform() ;

            QPainterPath tpath ;
            tpath.addText(pp, label_font_, text) ;
            painter->fillPath(tpath, label_brush_) ;
            painter->strokePath(tpath, label_pen_) ;

            painter->restore() ;

        }
    }
*/
}

GrabHandle *PolyRBand::getHandle(int i)  {
    return handles[i] ;
}

void PolyRBand::appendPoint(const QPointF &p, const QString &label) {
    poly.push_back(p) ;

    GrabHandle *pHandle = new GrabHandle( this) ;

    handles.push_back(pHandle) ;
    updatePoly() ;
}

void PolyRBand::removePoints(int start, int nPts)
{
    poly.remove(start, nPts) ;

    for(int i=start ; i<start + nPts ; i++ )
           delete handles[i] ;

    handles.remove(start, nPts) ;

    updatePoly() ;
}

int PolyRBand::whereIsPoint(const QPointF &pt)
{
    QGraphicsScene *sc = scene() ;
    QGraphicsItem *pItem = scene()->itemAt(pt, QTransform()) ;

    int idx = -1 ;
    for( int i=0 ; i<poly.size() ; i++ ) {
        if ( pItem == handles[i] ) {
            idx = i ;
            break ;
        }
    }

    return idx ;


}
