#ifndef XVIZ_QT_IMAGE_WIDGET_HPP
#define XVIZ_QT_IMAGE_WIDGET_HPP

#include <QWidget>
#include <QGraphicsView>
#include <QMenu>

class ImageGraphicsScene ;

class ImageWidget : public QGraphicsView
{
    Q_OBJECT

public:

    ImageWidget(QWidget *parent);

    QGraphicsScene *scene();

    // set canvas size
    void setCanvasSize(int sw, int sh) ;

    // get the selection of the rectangular rubber band
    QRect getRectSelection() const ;

    // get the selection of the polygon tool
    QPolygon getPolySelection() const ;

    // change the current selection of the polygon tool
    void setPolySelection(QPolygon &poly) ;

    bool hasImage() const {
        return has_image ;
    }

    // use this to add annotations over the image
    void addItem(QGraphicsItem *item) ;

    float getZoomFactor() const ;

    int getZoom() const {
        return zc ;
    }

    void zoomFit() ;


protected:

    QPixmap *pixmap ;
    QList<QGraphicsItem *> overlays ;
    QGraphicsPixmapItem *pixmapItem ;

public slots:

    void setZoom( int );
    void zoomRel( int ) ;

    void zoomToRect(const QRectF &rect) ;
    void zoomToPoint(const QPointF &pt, int delta) ;

    // set the image
    void setImage(const QImage &qim) ;
private:

    friend class ImageGraphicsScene ;

    void wheelEvent( QWheelEvent* );

    bool eventFilter(QObject *o, QEvent *e) ;


    void mousePressEventHandler ( QGraphicsSceneMouseEvent * mouseEvent ) ;
    void mouseMoveEventHandler ( QGraphicsSceneMouseEvent * mouseEvent ) ;
    void mouseReleaseEventHandler( QGraphicsSceneMouseEvent * mouseEvent ) ;


    ImageGraphicsScene *gscene ;
    QMenu *ctxMenu ;

    int zc ;
    bool has_image ;

signals:

    void zoomChanged(int) ;
};

class  ImageGraphicsScene: public QGraphicsScene
{
    Q_OBJECT

public:

    ImageGraphicsScene(ImageWidget *v): QGraphicsScene() {
        view = v ;
    }

signals:

    void fileDropped(QString) ;

private:

    void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent ) ;
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent ) ;
    void mouseReleaseEvent( QGraphicsSceneMouseEvent * mouseEvent ) ;

    void dragEnterEvent(QGraphicsSceneDragDropEvent *) ;
    void dropEvent(QGraphicsSceneDragDropEvent *);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *);



    ImageWidget *view ;
} ;










#endif
