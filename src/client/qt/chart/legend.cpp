#include <xviz/qt/chart/legend.hpp>

void Legend::draw(QPainter &c,  const QVector<LegendEntry> &entries, qreal w, qreal h)
{

    if ( entries.empty() ) return ;

    size_t nGraphs = entries.size() ;

    qreal lw = 0, lh = 0, ox, oy ;

    QVector<QRect> rects ;

    QFontMetrics fm(label_font_) ;

    for( size_t i=0 ; i<nGraphs ; i++ ) {
        QRect r(0, 0, max_label_width_, fm.height()) ;
        QRect br = fm.boundingRect(r, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, entries[i].label_) ;

        lw = std::max(lw, (qreal)br.width()) ;
        lh += std::max(min_row_height_, (qreal)br.height()) ;
        rects.append(br);
    }

    const double legend_gap = 4 ;

    // box dimensions

    lw = lw + preview_width_ + legend_gap + margin_ + margin_ ;
    lh += margin_ + margin_ ;

    // compute offset based on placement option

    switch ( placement_ ) {
    case TopRight:
        ox = w - lw - padding_ ;
        oy = -h + padding_ ;
        break ;
    case TopLeft:
        ox = padding_ ;
        oy = -h + padding_ ;
        break ;
    case TopCenter:
        ox = (w - lw)/2.0 ;
        oy = -h + padding_ ;
        break ;
    case BottomRight:
        ox = w - lw - padding_ ;
        oy = -lh -padding_ ;
        break ;
    case BottomLeft:
        ox = padding_ ;
        oy = -lh -padding_ ;
        break ;
    case BottomCenter:
        ox = (w - lw)/2.0 ;
        oy = -lh -padding_ ;
        break ;
    case CenterRight:
        ox = w - lw - padding_ ;
        oy = -(h - lh)/2.0 ;
        break ;
    case CenterLeft:
        ox = padding_ ;
        oy =-(h - lh)/2.0 ;
        break ;

    }

    QRect fr(ox, oy, lw, lh) ;

    ox += margin_ ;
    oy += margin_ ;

    c.save() ;
    c.setPen(bg_pen_) ;
    c.setBrush(bg_brush_) ;
    c.drawRect(fr) ;
    c.restore() ;

    for( size_t i=0 ; i<nGraphs ; i++ ) {
        const QRect &br = rects[i] ;

        double rh = std::max(min_row_height_, (qreal)br.height()) ;
        c.save() ;
        c.translate(ox, oy) ;
        drawPreview(c, preview_width_, rh, entries[i]) ;
        c.restore() ;
        QRect pr(ox + preview_width_ + legend_gap, oy, max_label_width_, rh) ;
        c.save() ;
        c.setFont(label_font_) ;
    //    c.setBrush(label_brush_);

        c.drawText(pr, Qt::AlignLeft|Qt::AlignVCenter|Qt::TextWordWrap, entries[i].label_) ;
        c.restore() ;
        oy += rh ;
    }

}


void Legend::drawPreview(QPainter &c, qreal width, qreal height, const LegendEntry &e) {
    double lh = height/2 ;

    c.save() ;
    c.setPen(e.pen_) ;
    c.setBrush(e.brush_) ;
    c.drawRect(0, lh-5, width, 10) ;
    c.restore() ;

    c.save() ;
    if ( !e.flags_ & LegendEntry::HasPen ) c.setPen(Qt::NoPen) ;
    else c.setPen(e.pen_) ;

    c.drawLine(0, lh, width, lh);
    c.restore() ;


    if ( e.flags_ & LegendEntry::HasMarker ) {
        c.save() ;
        c.translate(width/2, height/2) ;
        c.drawPixmap(-e.marker_.width()/2.f, -e.marker_.height()/2.f, e.marker_) ;
        c.restore() ;
    }

}


