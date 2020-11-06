#include "image_widget.hpp"

#include <QTimer>
#include <QToolButton>
#include <QCoreApplication>
#include <QMenu>
#include <QMouseEvent>
#include <QScrollBar>
#include <QApplication>
#include <QDesktopWidget>
#include <QPaintEngine>
#include <QDebug>
#include <QFileInfo>
#include <QUrl>
#include <QMimeData>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsPixmapItem>

using namespace std;

ImageWidget::ImageWidget(QWidget *parent): QGraphicsView(parent)
{
    setMouseTracking(true);
    setMinimumSize(200, 200) ;

    gscene = new ImageGraphicsScene(this) ;
    setScene(gscene) ;

    zc = 7 ;

    ctxMenu = NULL ;

    has_image = false ;
    pixmapItem = 0 ;
}


bool ImageWidget::eventFilter(QObject *o, QEvent *e)
{

    if (e->type() == QEvent::Wheel )
    {
        if ( o == (QObject *)verticalScrollBar() ) return true ;
        else if ( o == parent() )
        {
            QCoreApplication::sendEvent(this, e) ;
            return true ;
        }
        else return false ;
    }


    return QGraphicsView::eventFilter(o, e) ;

}



void ImageGraphicsScene::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{

    QGraphicsScene::mouseMoveEvent(mouseEvent) ;
    view->mouseMoveEventHandler( mouseEvent ) ;
}


void ImageWidget::mouseMoveEventHandler( QGraphicsSceneMouseEvent * mouseEvent ) {
    //if ( currentTool ) currentTool->mouseMoved(mouseEvent);
}


void ImageGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list") ||
            event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist") )
        event->acceptProposedAction();
}

void ImageGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
}

void ImageGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    QList<QUrl> urlList;
    QString fName;
    QFileInfo info;

    if (event->mimeData()->hasUrls())
    {
        urlList = event->mimeData()->urls(); // returns list of QUrls

        QStringList fnames ;

        // if just text was dropped, urlList is empty (size == 0)
        for( int i=0 ; i< urlList.size() ; i++ ) // if at least one QUrl is present in list
        {
            fName = urlList[i].toLocalFile(); // convert first QUrl to local path
            info.setFile( fName ); // information about file
            if ( info.isFile() ) fnames.append(fName) ;
        }

        emit fileDropped(fnames[0]) ;
    }

    event->acceptProposedAction();
}


void ImageGraphicsScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    QGraphicsScene::mousePressEvent(mouseEvent) ;
    view->mousePressEventHandler(mouseEvent) ;
}


void ImageWidget::mousePressEventHandler( QGraphicsSceneMouseEvent * mouseEvent )
{
    if ( mouseEvent->button() == Qt::RightButton && ctxMenu )
    {
        ctxMenu->popup(mouseEvent->screenPos()) ;
    }

 //   if ( currentTool ) currentTool->mousePressed(mouseEvent);

}

void ImageGraphicsScene::mouseReleaseEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
    QGraphicsScene::mouseReleaseEvent(mouseEvent) ;
    view->mouseReleaseEventHandler(mouseEvent) ;
}

void ImageWidget::mouseReleaseEventHandler( QGraphicsSceneMouseEvent * mouseEvent )
{
   // if ( currentTool ) currentTool->mouseReleased(mouseEvent);
}

QGraphicsScene *ImageWidget::scene() {
    return static_cast<QGraphicsScene *>(gscene) ;
}



QRect ImageWidget::getRectSelection() const
{
    return QRect() ;
    /*
    QRectTool *tool = qobject_cast<QRectTool *>(getCurrentTool()) ;

    if ( tool ) return tool->getRect().toRect() ;
    else return QRect() ;
    */
}

QPolygon ImageWidget::getPolySelection() const
{
    return QPolygon() ;
    /*
    QPolygonTool *tool = qobject_cast<QPolygonTool *>(getCurrentTool()) ;

    if ( tool ) return tool->getPolygon().toPolygon() ;
    else return QPolygon() ;
    */
}

void ImageWidget::setPolySelection(QPolygon &poly)
{
   // QPolygonTool *tool = qobject_cast<QPolygonTool *>(getCurrentTool()) ;

    //if ( tool ) tool->setPolygon(poly) ;
}

void ImageWidget::setImage(const QImage &qim)
{
    if ( pixmapItem ) delete pixmapItem ;
    pixmap = new QPixmap(QPixmap::fromImage(qim)) ;

    if ( !pixmap ) return  ;

    has_image = true ;

    pixmapItem = (QGraphicsPixmapItem *)gscene->addPixmap(*pixmap) ;
    pixmapItem->setZValue(-1.0) ;

    setSceneRect(QRectF(0, 0, pixmap->width(), pixmap->height())) ;

    verticalScrollBar()->installEventFilter(this);
}

void ImageWidget::addItem(QGraphicsItem *item)
{
    gscene->addItem(item) ;
}


static float zoomFactors[] = { 0.02, 0.05, 0.1, 0.25, 0.33, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0,
                               9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0
                             } ;

void ImageWidget::setZoom( int cc )
{
    if ( zc == cc ) return ;

    zc = cc;

    float zoomFactor = zoomFactors[cc] ;

    QTransform tr = transform() ;

    tr.setMatrix(zoomFactor, tr.m12(), tr.m13(), tr.m21(), zoomFactor, tr.m23(),
                 tr.m31(), tr.m32(), tr.m33()) ;

    setTransform(tr) ;

    emit zoomChanged(zc) ;
}

float ImageWidget::getZoomFactor() const {
    return zoomFactors[zc] ;
}

void ImageWidget::wheelEvent( QWheelEvent *event )
{
    int zz = zc  ;
    if ( event->delta() < 0 ) zz -- ;
    else zz++ ;

    if ( zz < 0 ) zz = 0 ;

    if( zz >= sizeof(zoomFactors)/sizeof(float)) zz = sizeof(zoomFactors)/sizeof(float)-1 ;

    setZoom( zz );
}

void ImageWidget::zoomRel( int delta )
{
    int zz = zc + delta   ;

    if ( zz < 0 ) zz = 0 ;

    if( zz >= sizeof(zoomFactors)/sizeof(float)) zz = sizeof(zoomFactors)/sizeof(float)-1 ;

    setZoom( zz );
}

void ImageWidget::zoomToPoint(const QPointF &p, int delta)
{
    int zz = zc + delta   ;

    if ( zz < 0 ) zz = 0 ;

    if( zz >= sizeof(zoomFactors)/sizeof(float)) zz = sizeof(zoomFactors)/sizeof(float)-1 ;

    if ( zc == zz ) return ;

    zc = zz;

    resetTransform() ;

    scale(zoomFactors[zc], zoomFactors[zc]) ;
    centerOn(p) ;

    emit zoomChanged(zc) ;
}

void ImageWidget::zoomToRect(const QRectF &rect)
{
    QRect srect = rect.toRect() ; //view->mapFromScene(rect).boundingRect() ;

    QSize sz = viewport()->size() ;
    QRect vpRect(0, 0, sz.width(), sz.height()) ;

    int cc = 0;
    for( cc = 0 ; cc < sizeof(zoomFactors)/sizeof(float) ; cc++ )
    {

        float zoomFactor = zoomFactors[cc] ;

        QTransform tr ;
        tr.translate(vpRect.center().x(), vpRect.center().y()) ;
        tr.scale(zoomFactor, zoomFactor) ;
        tr.translate(-rect.center().x(), -rect.center().y()) ;

        QRect zoomRect = tr.mapRect(rect).toRect() ;

        if ( zoomRect.isValid() && !vpRect.contains(zoomRect) ) break ;
    }

    cc = qMin(cc-1, (int)(sizeof(zoomFactors)/sizeof(float) - 1) ) ;
    cc = qMax(cc, 0) ;
    resetTransform() ;

    centerOn(rect.center()) ;
    scale(zoomFactors[cc], zoomFactors[cc]) ;

    if ( zc == cc ) return ;

    zc = cc;

    emit zoomChanged(zc) ;
}

void ImageWidget::setCanvasSize(int sw, int sh)
{
    setSceneRect(0, 0, sw, sh) ;
}
