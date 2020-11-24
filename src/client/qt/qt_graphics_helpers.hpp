#ifndef XVIZ_QT_GRAPHICS_HELPERS_HPP
#define XVIZ_QT_GRAPHICS_HELPERS_HPP

#include <QPen>
#include <QPixmap>
#include <QBrush>

#include <xviz/pen.hpp>
#include <xviz/marker.hpp>
#include <xviz/brush.hpp>
#include <xviz/path.hpp>
#include <xviz/image.hpp>
#include <xviz/font.hpp>

QPen qPenFromSolidPen(const xviz::Pen &pen);
QPixmap qPixmapFromMarker(const xviz::Marker &marker) ;
QBrush qBrushFromSolidBrush(const xviz::Brush &brush);

QPainterPath qPathFromPath(const xviz::Path &path) ;

QImage qImageFromImage(const xviz::Image &image) ;
QFont qFontFromFont(const xviz::Font &f) ;

#endif
