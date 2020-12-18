#ifndef XVIZ_QT_GRAPH_LEGEND_HPP
#define XVIZ_QT_GRAPH_LEGEND_HPP

#include <QPainter>
#include <QPen>
#include <QVariant>

struct LegendEntry {
    enum Flags { HasPen = 1, HasBrush = 2, HasMarker = 4 } ;

    int flags_ ;
    QPen pen_ ;
    QString label_ ;
    QBrush brush_ ;
    QPixmap marker_ ;
};


enum LegendPlacement { None, TopRight, TopLeft, TopCenter, BottomLeft, BottomCenter, BottomRight, CenterLeft, CenterRight } ;

class Legend {
public:
    Legend() {}

    void draw(QPainter &p, const QVector<LegendEntry> &entries, qreal w, qreal h) ;
    void drawPreview(QPainter &c, qreal w, qreal h, const LegendEntry &e);
    void setPlacement(LegendPlacement lg) { placement_ = lg ; }
private:

    qreal max_label_width_ = 150 ;
    qreal preview_width_ = 30 ;
    qreal min_row_height_ = 30;
    qreal padding_ = 6 ;
    qreal margin_ = 4 ;

    QFont label_font_ = QFont("Arial", 10) ;
    QBrush bg_brush_ = QBrush(Qt::white) ;
    QPen bg_pen_ = QPen() ;
    LegendPlacement placement_ = TopRight;
} ;

#endif
