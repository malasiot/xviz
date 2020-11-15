#include "axis.hpp"

#include <cmath>
using namespace std ;


TickFormatter Axis::nullFormatter =
        [](double v, int idx) { return QString() ; } ;

TickFormatter Axis::defaultFormatter =
        [](double v, int idx) {
    return QString("%1").arg(v, 0, 'g') ;
};

void Axis::computeAxisLayout(qreal ls, qreal wsize) {

    ls_ = ls ;

    title_extra_ = 0 ;

    if ( !title_.isEmpty() ) {
        QFontMetrics fm(title_font_) ;
        QRect lrect(0, 0, title_wrap_, fm.height()) ;
        title_extra_ = title_offset_ + fm.boundingRect(lrect, Qt::AlignHCenter | Qt::AlignTop, title_).height() ;
    }

    if ( ! has_fixed_tics_ ) {

        tick_locations_.clear() ;
        tick_labels_.clear() ;

        qreal s = wsize - 2*margin_ - ls  ;
        qreal sep ;

        // Compute number of tics for each axis based on the window dimensions

        unsigned numTics = 2 ;

        while (1) {
            sep = (s - ((numTics-1)*ls))/(numTics-1) ;
            if ( sep < label_sep_ ) break ;
            else numTics ++ ;
        }

        // compute scaling factor of displayed labels

        vscale_ = 1.0 ;
#if 0
        power_ = 0 ;

        if ( !is_log_ ) {
            double v = max(fabs(min_v_), fabs(max_v_)) ;
            while ( v * vscale_ <= 0.1 )  { vscale_ *= 10 ; ++power_ ; }
            while ( v * vscale_ > 10.0 ) { vscale_ /= 10 ; --power_ ; }
        }
#endif

        uint tics = numTics ;

        qreal _max = max_v_, _min = min_v_ ;

        if ( is_log_ )
        {
            if ( _min <= 0 ) throw std::runtime_error("log scale for negative number") ;
            else _min = log10(min_v_) ;

            _max = log10(_max) ;
        }


        tick_locator_->compute(_min, _max, numTics, is_log_ ? 1.0 : 0.0, min_label_v_, max_label_v_, tick_locations_) ;

        numTics = tick_locations_.size() ;

        // create labels

        tick_labels_.resize(numTics) ;

        QFontMetrics fm(label_font_) ;

        max_label_width_ = 0 ;
        for( uint i=0 ; i<numTics ; i++ ) {
            tick_labels_[i] = tick_formatter_(tick_locations_[i], i) ;
            if ( is_log_ ) max_label_width_ = 1.5*fm.width("10") ;
            else {
                max_label_width_ = std::max(max_label_width_, (qreal)fm.width(tick_labels_[i])) ;
            }
        }

        max_label_height_ = (is_log_) ? 1.25*fm.height() : fm.height() ;
    }
}

QRect Axis::paintLabel(QPainter &canvas,  const QString &text, qreal x, qreal y, bool rotate)
{
    QString mnt, expo ;
    int spos = text.indexOf('^') ;
    if ( spos != -1 )  {
        mnt = text.left(spos) ;
        expo = text.mid(spos+1) ;
    } else {
        mnt = text ;
    }

    QFontMetrics fm(label_font_);
    QRect lrect = fm.boundingRect(mnt);

    qreal lw = lrect.width() ;
    qreal lh = lrect.height() ;

    qreal xoffset = fm.averageCharWidth()/8.0;

    qreal soffset = 0.66*lrect.height() ;

    QFont superf(label_font_) ;
    int lf = label_font_.pointSize() ;
    superf.setPointSize(0.58 * lf) ;

    QRect srect ;
    qreal ew, eh ;

    if ( !expo.isEmpty() ) {
        QFontMetrics efm(superf) ;

        srect = efm.boundingRect(expo) ;
        ew = srect.width() ;
        eh = srect.height() ;

        lw += ew + xoffset;
        lh += soffset + eh - lh ;
    }

    double xf, yf ;
    if ( !rotate ) {
        xf = x - lw/2.0 ;
        yf = y ;
    } else {
        xf = x - lw ;
        yf = y - lh/2 ;
    }

    QRect layout_rect(xf, yf, lw, lh) ;

    canvas.save() ;
    canvas.setBrush(text_brush_) ;
    canvas.setFont(label_font_) ;

    canvas.drawText(layout_rect, Qt::AlignLeft | Qt::AlignBottom , mnt) ;

    if ( !expo.isEmpty() ) {

        canvas.setFont(superf) ;

        canvas.drawText(layout_rect, Qt::AlignRight | Qt::AlignTop, expo) ;
    }

    canvas.restore() ;

    return layout_rect ;
}

void XAxis::computeLayout(qreal wsize) {
    QFontMetrics fm(label_font_) ;
    int lw = fm.width("-0.09") ;
    computeAxisLayout(lw, wsize) ;
    size_ = label_offset_ + tic_size_ + max_label_height_ + title_extra_ ;

}

Axis &Axis::setTickLocations(const std::vector<double> &tp) {
    tick_locations_ = tp ;
    min_label_v_ = tp.front() ;
    max_label_v_ = tp.back() ;
    has_fixed_tics_ = true ;
    return *this ;
}

Axis &Axis::setTickLabels(const QVector<QString> &labels) {

    tick_labels_ = labels ;

    QFontMetrics fm(label_font_) ;

    max_label_width_ = 0 ;
    for( uint i=0 ; i<labels.size() ; i++ ) {
        max_label_width_ = std::max(max_label_width_, (qreal)fm.width(tick_labels_[i])) ;
    }

    max_label_height_ = fm.height() ;
}

void Axis::computeTransform(qreal ws) {
    scale_ = (ws - 2*margin_)/(max_label_v_ - min_label_v_) ;
    offset_ = -scale_ * min_label_v_  + margin_  ;
}

void XAxis::draw(QPainter &canvas, qreal wsize, qreal hsize) {

    unsigned s = wsize - 2 * margin_ ;
    unsigned nTics = tick_locations_.size() ;

    qreal ts = s/(nTics - 1) ;

    canvas.save() ;
    canvas.setFont(label_font_) ;
    canvas.setPen(tics_pen_) ;

    // draw axis line
    canvas.drawLine(0, 0, wsize, 0) ;

    qreal lb = 0 ;

    qreal ticy = ( tics_placement_ == TicsInside ) ? - tic_size_  :  tic_size_  ;
    qreal ticy_minor = ( tics_placement_ == TicsInside ) ? - tic_minor_size_  :  tic_minor_size_  ;
    qreal labely = ticy + (( tics_placement_ == TicsInside ) ? - label_offset_ :  label_offset_ ) ;

    // draw tics

    for(  uint j=0 ; j<nTics ; j++ ) {
        qreal x1 = transform(tick_locations_[j]) ;
        //qreal x1 = margin_ +  j * ts ;


        canvas.drawLine(x1, 0, x1, ticy) ;

        if ( is_log_ && j < nTics - 1) { // minor
            for( int k=2 ; k<10 ; k++ ) {
                qreal offset = ts*log10((qreal)k) ;
                canvas.drawLine(x1 + offset, 0, x1 + offset, ticy_minor) ;
            }
        }

        if ( is_log_ )
        {
            QRect r = paintLabel(canvas, "10^" + tick_labels_[j], x1,labely, false);
            lb = std::max((int)lb, r.height()) ;
        }
        else if ( !tick_labels_.isEmpty() )
        {
            QRect r = paintLabel(canvas, tick_labels_[j], x1,labely, false);
            lb = std::max((int)lb, r.height()) ;
        }

        if ( draw_grid_ ) {
            canvas.save() ;
            canvas.setPen(grid_pen_) ;
            canvas.drawLine(x1, 0, x1, -hsize) ;
            canvas.restore() ;
        }
    }


    // draw title

    if ( !title_.isEmpty() ) {
        qreal x1 = margin_ +  s/2 ;
        qreal y1 = lb + labely + title_offset_  ;

        QRect boundRect(x1 - title_wrap_/2, y1, title_wrap_, 1000) ;

        canvas.save() ;
        canvas.setFont(title_font_) ;
        canvas.setBrush(text_brush_) ;
        //canvas.setPen(EmptyPen()) ;
        canvas.drawText(boundRect, Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap, title_) ;
        canvas.restore() ;
    }


    canvas.restore() ;
}

void YAxis::computeLayout(double wsize) {
    QFontMetrics fm(label_font_) ;
    double maxLabelH = fm.width(fm.height()) ;
    computeAxisLayout(maxLabelH, wsize) ;
    size_ = tic_size_ + label_offset_ + max_label_width_ + title_extra_ ;
}

void YAxis::draw(QPainter &canvas, qreal wsize, qreal hsize) {

    unsigned s = hsize - 2 * margin_  ;
    unsigned nTics = tick_locations_.size() ;

    double ts = s/(nTics - 1) ;

    canvas.save() ;
    canvas.setFont(label_font_) ;
    canvas.setPen(tics_pen_) ;

    canvas.drawLine(0, 0, 0, -hsize) ;

    qreal lb = 0 ;

    qreal ticx = ( tics_placement_ == TicsInside ) ?  tic_size_  :  -tic_size_  ;
    qreal ticx_minor = ( tics_placement_ == TicsInside ) ?  tic_minor_size_  :  -tic_minor_size_ ;
    qreal labelx = ticx + (( tics_placement_ == TicsInside ) ?  label_offset_  :  -label_offset_ ) ;

    for(  int j=0 ; j<nTics ; j++ ) {
      //  qreal y1 = -j * ts - margin_ /*- (margin_ * gscale + s) */;

        qreal y1 = transform(tick_locations_[j]);

        canvas.drawLine(0, y1, ticx, y1) ;

        if ( is_log_ && j < nTics - 1) { // minor
            for( int k=2 ; k<10 ; k++ ) {
                qreal offset = ts*log10((qreal)k) ;
                canvas.drawLine(0, y1 + offset, ticx_minor, y1 + offset) ;
            }
        }

        if ( is_log_ )
        {
            QRect r = paintLabel(canvas, "10^" + tick_labels_[j], labelx, y1, true);
            lb = std::max(lb, (qreal)r.width()) ;
        }
        else
        {
            QRect r = paintLabel(canvas, tick_labels_[j], labelx, y1, true);
            lb = std::max(lb, (qreal)r.width()) ;
        }

        if ( draw_grid_ ) {
            canvas.save() ;
            canvas.setPen(grid_pen_) ;
            canvas.drawLine(0, y1, wsize, y1) ;
            canvas.restore() ;
        }

    }

    if ( !title_.isEmpty() ) {
        qreal y1 = -margin_ -  s/2 ;
        qreal x1 =  labelx - lb - title_offset_  ;

        canvas.save() ;
        canvas.translate(x1, y1) ;
        canvas.rotate(-90) ;
        canvas.translate(-x1, -y1) ;

        QRect boundRect(x1 - title_wrap_/2, y1 - 1000, title_wrap_, 1000) ;
        //    canvas.drawRect(boundRect) ;

        canvas.setFont(title_font_) ;
        canvas.setBrush(text_brush_) ;
        QRect bbr ;
        canvas.drawText(boundRect, Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, title_, &bbr) ;
        canvas.restore() ;

    }


    canvas.restore() ;
}

double XAxis::transform(double x) {
    if ( is_log_ ) {
        if ( x > 0.0 ) x = log10(x) ;
        else throw runtime_error("log scale needs positive numbers") ;
    }
    return scale_ * x + offset_ ;
}

double YAxis::transform(double y) {
    if ( is_log_ ) {
        if ( y > 0.0 ) y = log10(y) ;
        else throw runtime_error("log scale needs positive numbers") ;
    }
    return -(scale_ * y + offset_) ;
}

