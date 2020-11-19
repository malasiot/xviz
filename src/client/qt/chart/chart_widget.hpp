#ifndef XVIZ_QT_CHART_WIDGET_HPP
#define XVIZ_QT_CHART_WIDGET_HPP

#include <QWidget>
#include <memory>


class Chart ;

class ChartWidget: public QWidget {
    Q_OBJECT

public:
    ChartWidget(QWidget *parent) ;

    void setChart(Chart *c) { chart_.reset(c) ; update() ; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::unique_ptr<Chart> chart_ ;
};

class Chart {
public:

    Chart() = default ;
    virtual ~Chart() {}
    virtual void paint(QPainter &p, const QRect &rect) = 0 ;

    virtual QRectF getDataBounds() = 0 ;

    void setTitle(const QString &title) { title_ = title ; }
    const QString &getTitle() const { return title_ ; }
/*
    void setAnnotations(const std::vector<xviz::DrawableHandle> &drawables) {
        annotations_ = drawables ;
    }
    */

protected:
    QString title_ ;
    //std::vector<xviz::DrawableHandle> annotations_ ;
};


#endif
