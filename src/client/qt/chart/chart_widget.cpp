#include <xviz/qt/chart/chart_widget.hpp>
#include <xviz/qt/annotation.hpp>
#include <QPainter>

using namespace std ;

ChartWidget::ChartWidget(QWidget *parent): QWidget(parent)
{
    setMinimumSize(500, 300);
}

void ChartWidget::setChart(Chart *c) {
    chart_.reset(c) ;
    chart_->build() ;
    update() ;
}

void ChartWidget::paintEvent(QPaintEvent *)
{
    QRect rect = this->rect() ;

    QPainter painter(this) ;

    painter.setPen(Qt::red);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawRect(0, 0, rect.width()-1, rect.height()-1) ;

    const qreal margin = 10 ;

    painter.translate(margin, rect.height() - margin );
    if ( chart_ ) {
        chart_->paint(painter, QRect(0, 0, rect.width() - 2 * margin, rect.height() - 2 * margin)) ;
    }

}


void Chart::paint(QPainter &c, const QRect &rect) {

    c.setPen(Qt::black) ;

    qreal w = rect.width() ;
    qreal h = rect.height() ;
    qreal title_height = 0;

    // compute axis layout to determine valid plot rectangle

    if ( !title_.isEmpty() ) {
        QFontMetrics fm(title_font_) ;
        QRect layout(0, 0, max_title_width_, 100 ) ;
        QRect br = fm.boundingRect(layout, Qt::AlignHCenter|Qt::AlignTop|Qt::TextWordWrap, title_) ;
        h -= title_height = br.height() ;
    }

    uint cramp_width = 0 ;

    ColorRamp *cr = colorRamp() ;

    if ( cr ) {
        QRect r = cr->layout(h) ;
        cramp_width = r.width() ;
        w -= cramp_width ;
    }

    x_axis_.computeLayout(w) ;
    y_axis_.computeLayout(h) ;

    qreal ox = y_axis_.size() ;
    qreal oy = x_axis_.size() ;

    w -= ox ;
    h -= oy ;

    x_axis_.computeTransform(w) ;
    y_axis_.computeTransform(h) ;

    // draw

    c.save() ;
    c.translate(ox, -oy) ;

    // background

    c.fillRect(0, 0, w, -h, bg_brush_) ;

    // x-y axis

    x_axis_.draw(c, w, h);
    y_axis_.draw(c, w, h);

    // opposite lines

    c.drawLine(0, -h, w, -h) ;
    c.drawLine(w, 0, w, -h) ;

    // data

    c.save();
    c.setClipRect(0, 0, w, -h) ;

    paintChart(c, QSize(w, h)) ;

    xviz::Matrix2d m(x_axis_.getScale(), 0.0, 0.0, -y_axis_.getScale(), x_axis_.getOffset(), y_axis_.getOffset());

    paintAnnotations(c, m);

    c.restore() ;

    // color ramp

    if ( cr ) {
        c.save() ;
        c.translate(w, 0) ;
        cr->paint(c, QRect(0, 0, cramp_width, h));
        c.restore() ;
    }

    // legend

    legend_.draw(c, entries_, w, h);


    if ( !title_.isEmpty() ) {
        c.save() ;
        QRect layout(w/2- max_title_width_/2, -h - title_height - title_offset_, max_title_width_, 100 ) ;
        c.setFont(title_font_) ;
        c.drawText(layout, Qt::AlignHCenter|Qt::AlignTop|Qt::TextWordWrap, title_) ;
        c.restore() ;
    }

    c.restore() ;
}


Chart::Chart(const xviz::Chart *c)
{
    chart_.reset(c) ;
}

void Chart::build() {
    data_bounds_ = getDataBounds() ;

    x_axis_.setRange(data_bounds_.topLeft().x(), data_bounds_.bottomRight().x()) ;
    y_axis_.setRange(data_bounds_.topLeft().y(), data_bounds_.bottomRight().y()) ;

    x_axis_.setTitle(QString::fromStdString(chart_->labelX())) ;
    y_axis_.setTitle(QString::fromStdString(chart_->labelY())) ;

    if ( !chart_->getTicksX().empty() ) {
        vector<double> ticks ;
        QVector<QString> tick_labels ;
        for( const xviz::Tick &tick: chart_->getTicksX() ) {
            ticks.push_back(tick.pos_) ;
            if ( !tick.label_.empty() )
                tick_labels.append(QString::fromStdString(tick.label_)) ;
        }

        x_axis_.setTickLocations(ticks);
        if ( !tick_labels.isEmpty() )
            x_axis_.setTickLabels(tick_labels) ;
    }

    if ( !chart_->getTicksY().empty() ) {
        vector<double> ticks ;
        QVector<QString> tick_labels ;
        for( const xviz::Tick &tick: chart_->getTicksY() ) {
            ticks.push_back(tick.pos_) ;
            if ( !tick.label_.empty() )
                tick_labels.append(QString::fromStdString(tick.label_));
        }

        y_axis_.setTickLocations(ticks);
        if ( !tick_labels.isEmpty() )
            y_axis_.setTickLabels(tick_labels) ;
    }

    makeLegendEntries();

}

void Chart::paintAnnotations(QPainter &p, const xviz::Matrix2d &m) {
    AnnotationPainter ap(annotations_) ;
    ap.paint(p, m) ;
}
