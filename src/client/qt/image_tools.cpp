#include <xviz/qt/image_tools.hpp>

#include "image_rb.hpp"

#include <QGraphicsSceneEvent>

ZoomInTool::ZoomInTool(QGraphicsView *w): ImageTool(w) {
}

void ZoomInTool::mousePressed(QGraphicsSceneMouseEvent *mouseEvent) {
    emit zoomIn(mouseEvent->scenePos());
    //widget_->zoomToPoint(mouseEvent->scenePos(), +1) ;
}

/////////////////////////////////////////////////////////////////////////////

ZoomOutTool::ZoomOutTool(QGraphicsView *w): ImageTool(w) {
}


void ZoomOutTool::mousePressed(QGraphicsSceneMouseEvent *mouseEvent) {
     emit zoomOut(mouseEvent->scenePos());
    //widget_->zoomToPoint(mouseEvent->scenePos(), -1) ;
}

/////////////////////////////////////////////////////////////////////////////

ZoomRectTool::~ZoomRectTool() {
    delete rb_ ;
}

ZoomRectTool::ZoomRectTool(QGraphicsView *w): ImageTool(w) {}

void ZoomRectTool::mouseMoved(QGraphicsSceneMouseEvent *mouseEvent) {
    if ( mouseEvent->buttons() & Qt::LeftButton ) {
        rb_->setGeometry(QRect(origin_, widget_->mapFromScene(mouseEvent->scenePos())).normalized());
        widget_->ensureVisible(QRectF(mouseEvent->scenePos(), mouseEvent->scenePos()), 10, 10) ;
    }
}

void ZoomRectTool::mousePressed(QGraphicsSceneMouseEvent *mouseEvent) {
    rb_ = new QRubberBand(QRubberBand::Rectangle, widget_->viewport()) ;
    origin_scene_ = mouseEvent->scenePos() ;
    origin_ = widget_->mapFromScene(origin_scene_) ;
    rb_->setGeometry(QRect(origin_, QSize()));
    rb_->show();
}

void ZoomRectTool::mouseReleased(QGraphicsSceneMouseEvent *mouseEvent)
{
    rb_->hide();
    delete rb_ ;

    QRectF rz(origin_scene_, mouseEvent->scenePos()) ;

    emit zoomToRect(rz);
 //   widget_->zoomToRect(rz) ;
}


////////////////////////////////////////////////////////////////////////////////////

PanTool::PanTool(QGraphicsView *w): ImageTool(w) {}

void PanTool::mousePressed(QGraphicsSceneMouseEvent *mouseEvent)
{
    widget_->setDragMode(QGraphicsView::ScrollHandDrag) ;
}

void PanTool::mouseReleased(QGraphicsSceneMouseEvent *mouseEvent)
{
    widget_->setDragMode(QGraphicsView::NoDrag) ;
}

void PanTool::mouseMoved(QGraphicsSceneMouseEvent *mouseEvent) {

}


void RectTool::mousePressed(QGraphicsSceneMouseEvent *mouseEvent)
{
    if ( !rb_ ) return ;

    rb_->show() ;
    QPointF point = mouseEvent->scenePos() ;

    start_tracking_ = true ;

    if ( !edit_mode_ ) {
        rect_ = QRectF(point, point) ;
        rb_->setRect(rect_) ;
        emit rectChanged(rect_) ;
    }
    else if ( edit_motion_ == NONE )
    {
        edit_mode_ = false ;
        rect_ = QRectF(point, point) ;
        rb_->setRect(rect_) ;
        emit rectChanged(rect_) ;
    }
    else last_point_ = point ;

}

void RectTool::mouseReleased(QGraphicsSceneMouseEvent *event)
{
    if ( !rb_ ) return ;
    if (!edit_mode_) edit_mode_ = true ;


}


void RectTool::mouseMoved(QGraphicsSceneMouseEvent *event)
{
    if ( !rb_  ) return ;


    QGraphicsView *vw = widget_->scene()->views().front() ;

    QPointF point = event->scenePos() ;

    //QGraphicsPixmapItem *pitem = (QGraphicsPixmapItem *)vw->items()[0] ;

    qreal w = widget_->scene()->width() ;
    qreal h = widget_->scene()->height() ;

    point.rx() = qMin(w-1, point.x()) ;
    point.ry() = qMin(h-1, point.y()) ;
    point.rx() = qMax((qreal)0, point.x()) ;
    point.ry() = qMax((qreal)0, point.y()) ;

    if (edit_mode_ == false && ( event->buttons() & Qt::LeftButton ) )
    {
        rect_.setBottomRight(point) ;
        rb_->setRect(rect_) ;
        emit rectChanged(rect_) ;

        vw->ensureVisible(QRectF(point, point), 10, 10) ;
    }
    else if ( edit_mode_ == true && ( event->buttons() & Qt::LeftButton )  )
    {
        switch ( edit_motion_ )
        {
        case UPPER_LEFT:
            rect_.setTopLeft(point) ;
            break ;
        case UPPER_RIGHT:
            rect_.setTop(point.y()) ;
            rect_.setRight(point.x()) ;
            break ;
        case BOTTOM_LEFT:
            rect_.setBottom(point.y()) ;
            rect_.setLeft(point.x()) ;
            break ;
        case BOTTOM_RIGHT:
            rect_.setRight(point.x()) ;
            rect_.setBottom(point.y()) ;
            break ;
        case LEFT_SIDE:
            rect_.setLeft(point.x()) ;
            break ;
        case RIGHT_SIDE:
            rect_.setRight(point.x()) ;
            break ;
        case TOP_SIDE:
            rect_.setTop(point.y()) ;
            break ;
        case BOTTOM_SIDE:
            rect_.setBottom(point.y()) ;
            break ;
        case MOVE_RECT:
            rect_.translate(point - last_point_) ;
            break ;
        }

        if ( edit_motion_ != NONE ) rb_->setRect(rect_) ;
        last_point_ = point ;
        vw->ensureVisible(QRectF(point, point), 10, 10) ;
        emit rectChanged(rect_) ;

    }
    else
    {
        int idx  = rb_->whereIsPoint(point) ;

        for( int i=0 ; i<8 ; i++ )
        {
            rb_->getHandle(i)->setHighlighted(idx == i) ;
        }

        switch (idx)
        {
        case 0:
            edit_motion_ = UPPER_LEFT ; break ;
        case 1:
            edit_motion_ = TOP_SIDE ; break ;
        case 2:
            edit_motion_ = UPPER_RIGHT ; break ;
        case 3:
            edit_motion_ = RIGHT_SIDE ; break ;
        case 4:
            edit_motion_ = BOTTOM_RIGHT ; break ;
        case 5:
            edit_motion_ = BOTTOM_SIDE ; break ;
        case 6:
            edit_motion_ = BOTTOM_LEFT ; break ;
        case 7:
            edit_motion_ = LEFT_SIDE ; break ;
        case 8:
            edit_motion_ = MOVE_RECT ; break ;
        default:
            edit_motion_ = NONE ; break ;
        }
    }

 /*   char s[80] ;

    QRect r = rect_.normalized().toRect() ;
    sprintf(s, "(%d, %d) -> (%d, %d) [%dx%d]", r.left(), r.top(),
            r.right(), r.bottom(), r.width(), r.height()) ;
    emit showMessage(s) ;
*/
}



RectTool::RectTool(QGraphicsView *p): ImageTool(p) {
    rb_ = new RectRBand(nullptr, widget_->scene()) ;
    rb_->hide() ;

    start_tracking_ = false ;
    edit_mode_ = false ;
    edit_motion_ = NONE ;
}

RectTool::~RectTool() {
    delete rb_ ;
}

void RectTool::activate() {
    rb_->show() ;
}

void RectTool::deactivate() {
    rb_->hide() ;
}

void RectTool::setBrush(const QBrush &brush) {
    rb_->setBrush(brush) ;
}

void RectTool::setPen(const QPen &pen) {
    rb_->setPen(pen) ;
}


QRectF RectTool::getRect() const
{
    if ( rb_ ) return rb_->getRect() ;
    else return QRectF() ;
}

void RectTool::setRect(const QRectF &rect)
{
    if ( rb_ ) rb_->setRect(rect) ;
}

//////////////////////////////////////////////////////////////////////////////


PolygonTool::PolygonTool(QGraphicsView *p): ImageTool(p)
{
    start_tracking_ = false ;
    edit_mode_ = false ;
    edit_only_ = false ;
    index_ = -1 ;
    p_rz_ = nullptr ;

    rb_ = new PolyRBand((QGraphicsItem *)nullptr) ;

    widget_->scene()->addItem(rb_) ;
    rb_->hide() ;

    rb_flags_ = 0 ;
}

PolygonTool::~PolygonTool()
{
    delete rb_ ;
}

void PolygonTool::activate() {
    rb_->show() ;
}

void PolygonTool::deactivate() {
    rb_->hide() ;
}


void PolygonTool::mousePressed(QGraphicsSceneMouseEvent *mouseEvent)
{
    QPointF point = mouseEvent->scenePos() ;

    start_tracking_ = true ;

    int nPts = rb_->poly.size() ;

    if ( (mouseEvent->modifiers() & Qt::ShiftModifier) && !edit_only_ )
    {
        p_rz_ = new QRubberBand(QRubberBand::Rectangle, widget_->viewport()) ;
        start_move_ = point ;
        porigin_ = widget_->mapFromScene(point) ;
        p_rz_->setGeometry(QRect(porigin_, QSize()));
        p_rz_->show();

        for( int i=0 ; i<selected_.size() ; i++ )
        {
            index_ = selected_[i] ;
            rb_->getHandle(index_)->setHighlighted(false) ;
        }
        selected_.clear() ;
    }
    else if ( (mouseEvent->modifiers() & Qt::AltModifier) && !edit_only_ )
    {
        start_move_ = point ;
    }
    else if ( index_ == -1 && !edit_only_ )
    {
        QPointF lastPoint = point ;

        if ( nPts == 0 )
        {
            rb_->appendPoint(lastPoint, makeLabel(nPts++)) ;
            emit polygonChanged(rb_->poly) ;
        }
        else if ( max_pts_ == -1 || nPts < max_pts_ )
        {
            QPointF startPoint = ( nPts == 0 ) ? lastPoint : rb_->poly[nPts-1] ;
            rb_->appendPoint(point, makeLabel(nPts++)) ;
            emit polygonChanged(rb_->poly) ;
        }
    }
    else if ( (mouseEvent->modifiers() & Qt::ControlModifier) && !edit_only_ )
    {
        for(int i=0 ; i<selected_.size() ; i++ )
        {
            if ( selected_[i] == index_ )
            {
                selected_.remove(i) ;
                break ;
            }
        }

        if ( index_ == 0 && nPts == 1 )
        {
            rb_->removePoints(0) ;
            nPts -- ;
            emit polygonChanged(rb_->poly) ;
        }
        else if ( index_ == 0 && nPts <= 2 )
        {
            rb_->removePoints(0) ;
            nPts -- ;
           emit polygonChanged(rb_->poly) ;

        }
        else if ( index_ == nPts-1 && nPts<=2 )
        {
            rb_->removePoints(nPts-1) ;
            nPts -- ;
            emit polygonChanged(rb_->poly) ;
        }
        else
        {
            rb_->removePoints(index_) ;
            nPts-- ;
           emit polygonChanged(rb_->poly) ;

        }
    }
    else
    {
        edit_mode_ = true ;
    }

}

void PolygonTool::mouseReleased(QGraphicsSceneMouseEvent *mouseEvent)
{
    index_ = -1 ;

    edit_mode_ = false ;

    if ( p_rz_ )
    {
        p_rz_->hide();
        delete p_rz_ ;
        p_rz_ = nullptr ;

        QRectF rect = QRectF(mouseEvent->scenePos(), start_move_).normalized() ;

        for( int i=0 ; i<rb_->poly.size() ; i++ )
        {
            if ( rect.contains(rb_->poly[i]) )
            {
                selected_.append(i) ;
                rb_->getHandle(i)->setHighlighted() ;
            }
        }
    }
}

void PolygonTool::mouseMoved(QGraphicsSceneMouseEvent *mouseEvent)
{
    QPointF point = mouseEvent->scenePos() ;
    QGraphicsView *vw = (QGraphicsView *)widget_ ;

    qreal w = widget_->scene()->width() ;
    qreal h = widget_->scene()->height() ;

    point.rx() = qMin(w-1, point.x()) ;
    point.ry() = qMin(h-1, point.y()) ;
    point.rx() = qMax((qreal)0, point.x()) ;
    point.ry() = qMax((qreal)0, point.y()) ;

    unsigned nPts = rb_->poly.size() ;

    if ( !edit_mode_ ) index_ = -1 ;

    if ( (mouseEvent->modifiers() & Qt::ShiftModifier) && mouseEvent->buttons() & Qt::LeftButton )
    {
        p_rz_->setGeometry(QRect(porigin_, vw->mapFromScene(point)).normalized());
        vw->ensureVisible(QRectF(point, point), 10, 10) ;
    }
    else if ( (mouseEvent->modifiers() & Qt::AltModifier) && mouseEvent->buttons() & Qt::LeftButton )
    {
        QPointF resPoint = point ;

        if ( selected_.isEmpty() )
        {
            for(int i=0 ; i<nPts ; i++ )
            {
                rb_->poly[i].rx() += resPoint.x() - start_move_.x() ;
                rb_->poly[i].ry() += resPoint.y() - start_move_.y() ;
            }

        }
        else
        {
            for(int i=0 ; i<selected_.size() ; i++ )
            {
                int index = selected_[i] ;

                rb_->poly[index].rx() += resPoint.x() - start_move_.x() ;
                rb_->poly[index].ry() += resPoint.y() - start_move_.y() ;
            }
        }


        rb_->updatePoly() ;
       emit polygonChanged(rb_->poly) ;

        vw->ensureVisible(QRectF(point, point), 10, 10) ;

        start_move_ = point ;
    }
    else if (edit_mode_ == false && ( mouseEvent->buttons() & Qt::LeftButton ) )
    {
        rb_->poly[nPts-1] = point ;
        rb_->updatePoly() ;
        emit polygonChanged(rb_->poly) ;

        vw->ensureVisible(QRectF(point, point), 10, 10) ;

    }
    else if ( edit_mode_ && ( mouseEvent->buttons() & Qt::LeftButton ))
    {
        rb_->poly[index_] = point ;
        rb_->updatePoly() ;
        emit polygonChanged(rb_->poly) ;

        vw->ensureVisible(QRectF(point, point), 10, 10) ;

    }
    else
    {
        index_ = rb_->whereIsPoint(point) ;

        if ( index_ >= 0 )
        {
            if ( mouseEvent->modifiers() & Qt::ControlModifier ) {
                //widget_->setCursor(QCursor(QBitmap(":/images/")))
            }
            //      SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEALL))) ;
        }
    }


}


QPolygonF PolygonTool::getPolygon() const
{
    if ( rb_ ) return rb_->getPolygon() ;
    else return QPolygonF() ;
}

void PolygonTool::setPolygon(const QPolygonF &poly)
{
    if ( rb_ ) rb_->setPolygon(poly) ;
}

void PolygonTool::setPen(const QPen &pen)
{
    rb_->setPen(pen) ;
}

void PolygonTool::setBrush(const QBrush &brush)
{
    rb_->setBrush(brush) ;
}

