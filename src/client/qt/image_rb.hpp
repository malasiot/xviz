#pragma once

#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include <QFont>

class ImageWidget ;

class GrabHandle : public QGraphicsItem
{
public:

    GrabHandle(QGraphicsItem *parent);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPos(const QPointF &p) ;
    void setHighlighted(bool h = true) ;
    void setSmall(bool sm=true) ;

    QPointF pos ;

protected:

    QRectF sceneRect() const;

private:

    bool highlighted ;
    bool isSmall ;
} ;

class RectRBand : public QGraphicsItemGroup
{
public:

    RectRBand(QGraphicsItem *parent, QGraphicsScene *scene);
    ~RectRBand();

    void setPen(QPen pen) { pen_ = pen ; }
    void setBrush(QBrush brush) { brush_ = brush ; }

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setRect(const QRectF &r) ;
    QRect getRect() const { return rect.toRect() ; }

    // -1: means outside, 0-7 means handle, 8 means inside
    int whereIsPoint(const QPointF &p) ;

    GrabHandle *getHandle(int i) ;

private:

    friend class RectTool ;
    void hideAll() ;
    void showAll() ;

    QRectF rect, brect ;
    GrabHandle *handle[8] ;
    QPen pen_ ;
    QBrush brush_ ;
};

class PolyRBand : public QGraphicsItem
{
public:

    PolyRBand(QGraphicsItem *parent) ;

    QPolygonF getPolygon() const ;
    void setPolygon(const QPolygonF &poly) ;

    void setPen(const QPen &pen) ;
    void setDragPen(const QPen &pen) ;
    void setBrush(const QBrush &brush) ;

    ~PolyRBand() = default ;

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    GrabHandle *getHandle(int i) ;
    void appendPoint(const QPointF &p, const QString &label) ;
    void removePoints(int start, int nPts = 1) ;

    int whereIsPoint(const QPointF &pt) ;

private:

    friend class PolygonTool ;

    QPen pen_, drag_pen_ ;
    QBrush brush_;

    QRectF brect ;
    QPolygonF poly ;
    QVector<GrabHandle *> handles ;

    void updatePoly() ;
};
