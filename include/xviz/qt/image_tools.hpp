#pragma once

#include <xviz/qt/image_widget.hpp>

class QGraphicsSceneMouseEvent ;
class RectRBand ;
class QRubberBand ;


#include <QRubberBand>

class ZoomInTool: public ImageTool
{
    Q_OBJECT

public:
    ZoomInTool(QGraphicsView *w) ;

signals:
    void zoomIn(const QPointF &p);

protected:
    void mousePressed(QGraphicsSceneMouseEvent *) override ;
    void mouseReleased(QGraphicsSceneMouseEvent *) override {}
    void mouseMoved(QGraphicsSceneMouseEvent *) override {}

} ;


class ZoomOutTool: public ImageTool
{
    Q_OBJECT

public:
    ZoomOutTool(QGraphicsView *w) ;

signals:
    void zoomOut(const QPointF &p);

protected:

    void mousePressed(QGraphicsSceneMouseEvent *) override ;
    void mouseReleased(QGraphicsSceneMouseEvent *) override {}
    void mouseMoved(QGraphicsSceneMouseEvent *) override {}
} ;


class ZoomRectTool: public ImageTool
{
    Q_OBJECT

public:
    ZoomRectTool(QGraphicsView *w) ;
    ~ZoomRectTool();

signals:
    void zoomToRect(const QRectF &r) ;

protected:

    void mousePressed(QGraphicsSceneMouseEvent *) override;
    void mouseReleased(QGraphicsSceneMouseEvent *) override;
    void mouseMoved(QGraphicsSceneMouseEvent *) override;

private:

    QRubberBand *rb_ = nullptr ;
    QPoint origin_ ;
    QPointF origin_scene_ ;
} ;


class PanTool: public ImageTool
{
    Q_OBJECT

public:
    PanTool(QGraphicsView *w) ;

protected:

    void mousePressed(QGraphicsSceneMouseEvent *) override;
    void mouseReleased(QGraphicsSceneMouseEvent *) override;
    void mouseMoved(QGraphicsSceneMouseEvent *) override ;

} ;


class RectTool: public ImageTool
{
    Q_OBJECT

public:

    RectTool(QGraphicsView *p) ;
    ~RectTool();

    void setRect(const QRectF &rect) ;
    QRectF getRect() const ;

    void setPen(const QPen &pen) ;
    void setBrush(const QBrush &brush) ;

signals:

    void rectChanged(const QRectF &rect) ;

protected:

    virtual void activate() override ;
    virtual void deactivate() override ;

    virtual void mousePressed(QGraphicsSceneMouseEvent *pevent) override ;
    virtual void mouseReleased(QGraphicsSceneMouseEvent *pevent) override ;
    virtual void mouseMoved(QGraphicsSceneMouseEvent *pevent) override ;

    RectRBand *rb_ ;

private:

    QPointF last_point_, p_initial_, p_final_;

    bool start_tracking_ ;
    bool edit_mode_ ;
    QRectF rect_ ;

    enum EditMotionType { UPPER_LEFT, UPPER_RIGHT, BOTTOM_LEFT,
                          BOTTOM_RIGHT, TOP_SIDE, BOTTOM_SIDE, LEFT_SIDE, RIGHT_SIDE, MOVE_RECT,
                          NONE } edit_motion_ ;
} ;

class PolyRBand ;

class PolygonTool: public ImageTool
{
    Q_OBJECT

public:
    PolygonTool(QGraphicsView *p) ;
    ~PolygonTool();

    QPolygonF getPolygon() const ;
    void setPolygon(const QPolygonF &poly) ;

    void setPen(const QPen &pen) ;
    void setBrush(const QBrush &brush) ;

    void setLabelPen(const QPen &pen) ;
    void setLabelBrush(const QBrush &brush) ;
    void setLabelFont(const QFont &font) ;

    void drawLabels(bool draw = true) ;
    void drawClosed(bool draw = true) ;
    void setEditOnly(bool edit = true) ;

    void setMaxPoints(int max_pts = -1) {
        max_pts_ = max_pts ;
    }

    // called when a new point is added to obtain the associated label
    virtual QString makeLabel(int i) const {
        return QString::number(i) ;
    }

signals:
    void polygonChanged(const QPolygonF &poly);

protected:
    virtual void activate() override ;
    virtual void deactivate() override ;

    virtual void mousePressed(QGraphicsSceneMouseEvent *pevent) override;
    virtual void mouseReleased(QGraphicsSceneMouseEvent *pevent) override;
    virtual void mouseMoved(QGraphicsSceneMouseEvent *pevent) override;

private:

    PolyRBand *rb_ ;

    QPointF start_move_ ;
    QRubberBand *p_rz_ ;
    QPoint porigin_ ;
    QVector<int> selected_ ;

    bool start_tracking_ ;
    int  index_ ;
    bool edit_mode_ ;
    bool edit_only_ ;
    int rb_flags_ ;

    int max_pts_ = -1;
} ;
