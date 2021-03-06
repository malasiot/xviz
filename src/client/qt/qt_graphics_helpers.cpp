#include "qt_graphics_helpers.hpp"

#include <cmath>

#include <xviz/pen.hpp>

#include <QBitmap>
#include <QPen>
#include <QPainter>

QPen qPenFromSolidPen(const xviz::Pen &pen) {
    if ( pen.type() == xviz::NoPen ) return QPen(Qt::NoPen) ;
    else {
        QPen qpen ;

        auto color = pen.lineColor() ;
        qpen.setColor(QColor(color.r() * 255, color.g() * 255, color.b() * 255, color.a()*255));

        switch ( pen.lineCap() ) {
        case xviz::LineCap::Square:
            qpen.setCapStyle(Qt::SquareCap) ;
            break ;
        case xviz::LineCap::Round:
            qpen.setCapStyle(Qt::RoundCap) ;
            break ;
        case xviz::LineCap::Butt:
            qpen.setCapStyle(Qt::FlatCap) ;
            break ;
        }

        switch ( pen.lineJoin() ) {
        case xviz::LineJoin::Bevel:
            qpen.setJoinStyle(Qt::BevelJoin) ;
            break ;
        case xviz::LineJoin::Miter:
            qpen.setJoinStyle(Qt::MiterJoin) ;
            break ;
        case xviz::LineJoin::Round:
            qpen.setJoinStyle(Qt::RoundJoin) ;
            break ;
        }

        switch ( pen.lineStyle() ) {
        case xviz::SolidLine:
            qpen.setStyle(Qt::SolidLine);
            break ;
        case xviz::DashLine:
            qpen.setStyle(Qt::DashLine);
            break ;
        case xviz::DotLine:
            qpen.setStyle(Qt::DotLine);
            break ;
        case xviz::DashDotLine:
            qpen.setStyle(Qt::DashDotLine);
            break ;
        case xviz::CustomDashLine: {
            qpen.setDashOffset(pen.dashOffset());
            QVector<qreal> dp ;
            for( const float &v: pen.dashArray() ) dp.append(v);
            qpen.setDashPattern(dp);
            break ;
        }

        }

        qpen.setWidth(pen.lineWidth());

        return qpen ;
    }
}

QTransform qTransformFromMatrix(const xviz::Matrix2d &mat) {
    return QTransform(mat.m1(), mat.m2(), mat.m3(), mat.m4(), mat.m5(), mat.m6()) ;
}

QBrush qBrushFromSolidBrush(const xviz::Brush &brush) {
    switch ( brush.type() ) {
    case xviz::BrushType::NoBrush:
        return QBrush(Qt::NoBrush) ;
    case xviz::BrushType::SolidBrush: {
        QBrush qbrush ;
        qbrush.setStyle(Qt::SolidPattern) ;
        auto color = brush.color() ;
        qbrush.setColor(QColor(color.r() * 255, color.g() * 255, color.b() * 255, color.a()*255));
        return qbrush ;
    }
    case xviz::BrushType::LinearGradientBrush: {
        QLinearGradient qlg ;

        const xviz::LinearGradient &lg = brush.linearGradient();
        qlg.setStart(QPointF(lg.startPoint().x(), lg.startPoint().y()));
        qlg.setFinalStop(QPointF(lg.finishPoint().x(), lg.finishPoint().y()));
        switch ( lg.spread() ) {
        case xviz::SpreadMethod::Pad:
            qlg.setSpread(QLinearGradient::PadSpread);
            break ;
        case xviz::SpreadMethod::Repeat:
            qlg.setSpread(QLinearGradient::RepeatSpread);
            break ;
        case xviz::SpreadMethod::Reflect:
            qlg.setSpread(QLinearGradient::ReflectSpread);
            break ;
        }

        switch ( lg.units() ) {
        case xviz::GradientUnits::ObjectBoundingBox:
            qlg.setCoordinateMode(QGradient::CoordinateMode::ObjectBoundingMode);
            break ;
        case xviz::GradientUnits::UserSpaceOnUse:
            qlg.setCoordinateMode(QGradient::CoordinateMode::LogicalMode);
            break ;
        }

        for( const auto &stop: lg.stops() ) {
            qlg.setColorAt(stop.position(), QColor(stop.color().r() * 255, stop.color().g() * 255, stop.color().b() * 255, stop.color().a()*255));
        }

        QBrush qbrush(qlg) ;
        qbrush.setTransform(qTransformFromMatrix(lg.transform())) ;
        return qbrush ;
    }
    case xviz::BrushType::RadialGradientBrush: {
        QRadialGradient qrg ;

        const xviz::RadialGradient &rg = brush.radialGradient();
        qrg.setCenter(QPointF(rg.center().x(), rg.center().y()));
        qrg.setFocalPoint(QPointF(rg.focal().x(), rg.focal().y()));
        qrg.setCenterRadius(rg.radius()) ;
        switch ( rg.spread() ) {
        case xviz::SpreadMethod::Pad:
            qrg.setSpread(QLinearGradient::PadSpread);
            break ;
        case xviz::SpreadMethod::Repeat:
            qrg.setSpread(QLinearGradient::RepeatSpread);
            break ;
        case xviz::SpreadMethod::Reflect:
            qrg.setSpread(QLinearGradient::ReflectSpread);
            break ;
        }

        switch ( rg.units() ) {
        case xviz::GradientUnits::ObjectBoundingBox:
            qrg.setCoordinateMode(QGradient::CoordinateMode::ObjectBoundingMode);
            break ;
        case xviz::GradientUnits::UserSpaceOnUse:
            qrg.setCoordinateMode(QGradient::CoordinateMode::LogicalMode);
            break ;
        }

        for( const auto &stop: rg.stops() ) {
            qrg.setColorAt(stop.position(), QColor(stop.color().r() * 255, stop.color().g() * 255, stop.color().b() * 255, stop.color().a()*255));
        }

        QBrush qbrush(qrg) ;
        qbrush.setTransform(qTransformFromMatrix(rg.transform())) ;
        return qbrush ;
    }

    }
}

QPixmap qPixmapFromMarker(const xviz::Marker &m)
{
    if ( m.type() == xviz::StockShapeMarker ) {
        int sz = std::round(m.size()) + 4;

        QPixmap pm(sz, sz);
        pm.fill(Qt::transparent);
        QPainter p(&pm) ;

        p.setPen(qPenFromSolidPen(m.pen())) ;
        QBrush qbrush = qBrushFromSolidBrush(m.brush()) ;
        p.setRenderHint( QPainter::Antialiasing );

        QPainterPath path ;
        const qreal hsz = sz/2.0f  ;
        const qreal szy = hsz * sqrt((double)5)/2.0 ;

        p.translate(hsz, hsz) ;

        switch ( m.shape() ) {
        case xviz::StockMarkerShape::Circle:
            path.addEllipse(QPointF(0, 0), hsz, hsz) ;
            break ;
        case xviz::StockMarkerShape::Point:
            path.addEllipse(QPointF(0, 0), 1, 1) ;
            break ;
        case xviz::StockMarkerShape::Square:
            path.addRect(-hsz, -hsz, 2*hsz, 2*hsz) ;
            break ;
        case xviz::StockMarkerShape::Plus:
            path.moveTo(-hsz, 0) ; path.lineTo(hsz, 0) ;
            path.moveTo(0, -hsz) ; path.lineTo(0, hsz) ;
            break ;
        case xviz::StockMarkerShape::XMark:
            path.moveTo(-hsz, -hsz) ; path.lineTo(hsz, hsz) ;
            path.moveTo(-hsz, hsz) ; path.lineTo(hsz, -hsz) ;
            break ;
        case xviz::StockMarkerShape::Star:
            path.moveTo(-hsz, 0) ; path.lineTo(hsz, 0) ;
            path.moveTo(0, -hsz) ; path.lineTo(0, hsz) ;
            path.moveTo(-hsz, -hsz); path.lineTo(hsz, hsz) ;
            path.moveTo(hsz, -hsz) ; path.lineTo(-hsz, hsz);
            break ;
        case xviz::StockMarkerShape::Diamond:
            path.moveTo(0, -hsz) ; path.lineTo(-hsz, 0) ;
            path.lineTo(0, hsz) ; path.lineTo(hsz, 0) ;
            path.closeSubpath() ;
            break ;
        case xviz::StockMarkerShape::TriangleUp:
            path.moveTo(0, -szy) ;
            path.lineTo(-hsz, szy);
            path.lineTo(hsz, szy) ;
            path.lineTo(0, -szy) ;
            break ;
        case xviz::StockMarkerShape::TriangleDown:
            path.moveTo(-hsz, -szy) ; path.lineTo(0, szy) ;
            path.lineTo(hsz, -szy) ; path.lineTo(-hsz, -szy) ;
            break ;
        case xviz::StockMarkerShape::TriangleLeft:
            path.moveTo(-szy, 0);
            path.lineTo(szy, -hsz);
            path.lineTo(szy, hsz);
            path.lineTo(-szy, 0) ;

            break ;
        case xviz::StockMarkerShape::TriangleRight:
            path.moveTo(szy, 0);
            path.lineTo(-szy, -hsz);
            path.lineTo(-szy, hsz);
            path.lineTo(szy, 0) ;
            break ;
        }

        p.fillPath(path, qbrush) ;
        p.drawPath(path) ;

        return pm ;

    } else {
        return QPixmap() ;
    }
}

QFont qFontFromFont(const xviz::Font &f) {
    QFont font ;
    QStringList family_names ;

//    for( const auto &fam: f.familyNames() )
//        family_names.append(QString::fromStdString(fam)) ;
//    font.setFamilies(family_names) ;

    font.setFamily(QString::fromStdString(f.familyNames()[0]));
    font.setPointSizeF(f.size()) ;
    return font ;
}
#if 0
void paintDrawable(QPainter &p, xviz::DrawableHandle drawable,
                   double tx, double ty, double sx, double sy)
{
    p.save() ;

    auto pen = drawable->pen() ;
    auto brush = drawable->brush() ;

    p.setPen(qPenFromSolidPen(pen)) ;
    p.setBrush(qBrushFromSolidBrush(brush));

    xviz::Matrix2d mat(sx, 0, 0, sy, tx, ty) ;

    if ( const xviz::ShapeDrawable *sd = dynamic_cast<const xviz::ShapeDrawable *>(drawable.get()) ) {
        QPainterPath pp(qPathFromPath(sd->path().transformed(mat))) ;
        p.drawPath(pp) ;
    }

    p.restore() ;
}
#endif
QPainterPath qPathFromPath(const xviz::Path &path)
{
    QPainterPath pp ;

    for( const auto &cmd: path.data().commands() ) {
        switch ( cmd.cmd_ ) {
        case xviz::PathData::MoveToCmd:
            pp.moveTo(cmd.arg0_, cmd.arg1_) ;
            break ;
        case xviz::PathData::LineToCmd:
            pp.lineTo(cmd.arg0_, cmd.arg1_) ;
            break ;
        case xviz::PathData::CurveToCmd:
            pp.cubicTo(cmd.arg0_, cmd.arg1_, cmd.arg2_, cmd.arg3_, cmd.arg4_, cmd.arg5_) ;
            break ;
        case xviz::PathData::ClosePathCmd:
            pp.closeSubpath() ;
        }
    }

    return pp ;

}

QImage qImageFromImage(const xviz::Image &image) {
    if ( image.format() == xviz::ImageFormat::encoded )
        return QImage::fromData(image.data(), image.dataSize());
    else if ( image.format() == xviz::ImageFormat::rgba32 ) {
        QImage qim(image.data(), image.width(), image.height(), QImage::Format_RGBA8888) ;
        qim.detach() ;
        return qim ;
    }
}
