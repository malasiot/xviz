#ifndef XVIZ_PATH_HPP
#define XVIZ_PATH_HPP

#include <string>
#include <memory>
#include <vector>

#include <xviz/xform.hpp>
#include <xviz/rectangle.hpp>

namespace xviz {

namespace msg {
    class Path ;
}

class PathData ;
class PathBuilder ;

class Path
{
public:

    Path() ;
    ~Path() = default;

    // See SVG 1.1 Path specification (http://www.w3.org/TR/SVG/paths.html)

    PathBuilder builder() ;

    Path transformed(const Matrix2d &m) const;

    // path bounding box
    Rectangle2d extents() const ;

    // return a flattened version of the path
    Path flattened() const ;

    const PathData &data() const { return *data_ ; }

    static msg::Path *write(const Path &path) ;
    static Path read(const msg::Path &p) ;

private:

  std::shared_ptr<PathData> data_ ;

} ;

class PathData {
public:

    enum Command { MoveToCmd, ClosePathCmd, LineToCmd, CurveToCmd, QuadCurveToCmd } ;

    struct CommandBlock {
        CommandBlock(Command cmd, double arg0=0, double arg1=0, double arg2=0, double arg3=0, double arg4=0, double arg5=0):
            cmd_(cmd), arg0_(arg0), arg1_(arg1), arg2_(arg2), arg3_(arg3), arg4_(arg4), arg5_(arg5) {}

        double arg0_, arg1_, arg2_, arg3_, arg4_, arg5_ ;

        Command cmd_ ;
    } ;

    const std::vector<CommandBlock> &commands() const { return cmds_ ; }

    void addCommand(Command cmd, double arg0=0, double arg1=0, double arg2=0, double arg3=0, double arg4=0, double arg5=0) ;

private:
    friend class PathBuilder ;
    friend class Path ;

    std::vector<CommandBlock> cmds_ ;
};

class PathBuilder {

    friend class Path ;

    PathBuilder(PathData &data) ;
public:
    PathBuilder & moveTo(double x, double y) ;
    PathBuilder & moveToRel(double x, double y) ;

    PathBuilder & lineTo(double x, double y) ;
    PathBuilder & lineToRel(double x, double y) ;

    PathBuilder & lineToHorz(double x) ;
    PathBuilder & lineToHorzRel(double x) ;

    PathBuilder & lineToVert(double y) ;
    PathBuilder & lineToVertRel(double y) ;

    PathBuilder & curveTo(double x, double y, double x1, double y1, double x2, double y2) ;
    PathBuilder & curveToRel(double x, double y, double x1, double y1, double x2, double y2) ;

    PathBuilder & quadTo(double x, double y, double x1, double y1) ;
    PathBuilder & quadToRel(double x, double y, double x1, double y1) ;

    PathBuilder & smoothCurveTo(double x, double y, double x1, double y1) ;
    PathBuilder & smoothCurveToRel(double x, double y, double x1, double y1) ;

    PathBuilder & smoothQuadTo(double x, double y) ;
    PathBuilder & smoothQuadToRel(double x, double y) ;

    PathBuilder & arcTo(double rx, double ry, double angle, bool largeArcFlag, bool sweepFlag, double x, double y) ;
    PathBuilder & arcToRel(double rx, double ry, double angle, bool largeArcFlag, bool sweepFlag, double x, double y) ;

    PathBuilder & closePath() ;

    PathBuilder & addEllipse(double x0, double y0, double r1, double r2) ;
    // The arc is traced along the perimeter of the ellipse bounded by the specified rectangle.
    // The starting point of the arc is determined by measuring clockwise from the x-axis of the
    // ellipse (at the 0-degree angle) by the number of degrees in the start angle.
    // The endpoint is similarly located by measuring clockwise from the starting point by the
    // number of degrees in the sweep angle.
    PathBuilder & addArc(double x0, double y0, double r1, double r2, double startAngle, double sweepAngle) ;
    PathBuilder & addRect(double x0, double y0, double w, double h) ;
    PathBuilder & addRoundedRect(double x0, double y0, double w, double h, double xrad, double yrad) ;
    PathBuilder & addPath(const Path &other) ;
//    Path & addText(const std::string &str, double x0, double y0, const Font &font) ;
//    Path & addGlyphs(const std::vector<Glyph> &glyphs, const std::vector<Point2d> &pos, const Font &font) ;


    PathBuilder &addPolygon(const std::vector<Point2d> &pts) ;
    PathBuilder &addPolyline(const std::vector<Point2d> &pts) ;

private:

    void addCommand(PathData::Command cmd, double arg0=0, double arg1=0, double arg2=0, double arg3=0, double arg4=0, double arg5=0) ;

    PathData &data_ ;

    double cx_, cy_, rx_, ry_ ;
    PathData::Command previous_cmd_ ;

};


} // namespace xviz ;


#endif
