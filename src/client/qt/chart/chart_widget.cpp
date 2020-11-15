#include "chart_widget.hpp"

#include <QPainter>

ChartWidget::ChartWidget(QWidget *parent): QWidget(parent)
{
    setMinimumSize(500, 300);
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
