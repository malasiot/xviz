#include <xviz/path.hpp>

#include "drawable.pb.h"

#include <cmath>

using namespace std ;

namespace xviz {

void PathData::addCommand(Command cmd, double arg0, double arg1, double arg2, double arg3, double arg4, double arg5)
{
    switch ( cmd )
    {
    case MoveToCmd:
    case LineToCmd:
        cmds_.emplace_back(cmd, arg0, arg1) ;
        break ;
    case CurveToCmd:
        cmds_.emplace_back(cmd, arg0, arg1, arg2, arg3, arg4, arg5) ;
        break ;
    case ClosePathCmd:
        cmds_.emplace_back(cmd) ;
        break ;
    }

    //previous_cmd_ = cmd ;
}


static void svg_path_arc_segment (double ctx[6],
double xc, double yc,
double th0, double th1, double rx, double ry, double x_axis_rotation)
{
    double sin_th, cos_th;
    double a00, a01, a10, a11;
    double x1, y1, x2, y2, x3, y3;
    double t;
    double th_half;

    sin_th = sin (x_axis_rotation * (M_PI / 180.0));
    cos_th = cos (x_axis_rotation * (M_PI / 180.0));
    /* inverse transform compared with rsvg_path_arc */
    a00 = cos_th * rx;
    a01 = -sin_th * ry;
    a10 = sin_th * rx;
    a11 = cos_th * ry;

    th_half = 0.5 * (th1 - th0);
    t = (8.0 / 3.0) * sin (th_half * 0.5) * sin (th_half * 0.5) / sin (th_half);
    x1 = xc + cos (th0) - t * sin (th0);
    y1 = yc + sin (th0) + t * cos (th0);
    x3 = xc + cos (th1);
    y3 = yc + sin (th1);
    x2 = x3 + t * sin (th1);
    y2 = y3 - t * cos (th1);

    ctx[0] = a00 * x1 + a01 * y1 ;
    ctx[1] = a10 * x1 + a11 * y1 ;
    ctx[2] = a00 * x2 + a01 * y2 ;
    ctx[3] = a10 * x2 + a11 * y2 ;
    ctx[4] = a00 * x3 + a01 * y3 ;
    ctx[5] = a10 * x3 + a11 * y3;
}

PathBuilder::PathBuilder(PathData &data): data_(data) {

}

PathBuilder & PathBuilder::moveTo(double x, double y) {
    addCommand(PathData::MoveToCmd, cx_ = x, cy_ = y) ;
    return *this ;
}

PathBuilder & PathBuilder::moveToRel(double x, double y) {
    cx_ += x ; cy_ += y ;
    addCommand(PathData::MoveToCmd, cx_, cy_) ;
    return *this ;
}

PathBuilder & PathBuilder::closePath() {
    addCommand(PathData::ClosePathCmd) ;
    return *this ;
}

PathBuilder & PathBuilder::lineTo(double x, double y) {
    addCommand(PathData::LineToCmd, cx_ = x, cy_ = y) ;
    return *this ;
}

PathBuilder & PathBuilder::lineToRel(double x, double y) {
    cx_ += x ; cy_ += y ;
    addCommand(PathData::LineToCmd, cx_, cy_) ;
    return *this ;
}

PathBuilder & PathBuilder::lineToHorz(double x) {
    addCommand(PathData::LineToCmd, cx_ = x, cy_) ;
    return *this ;
}

PathBuilder & PathBuilder::lineToHorzRel(double x) {
    addCommand(PathData::LineToCmd, cx_ += x, cy_) ;
    return *this ;
}

PathBuilder & PathBuilder::lineToVert(double y) {
    addCommand(PathData::LineToCmd, cx_, cy_ = y) ;
    return *this ;
}

PathBuilder & PathBuilder::lineToVertRel(double y) {
    addCommand(PathData::LineToCmd, cx_, cy_ += y) ;
    return *this ;
}

PathBuilder & PathBuilder::curveTo(double x, double y, double x1, double y1, double x2, double y2) {
    addCommand(PathData::CurveToCmd, x, y, rx_ = x1, ry_ = y1, cx_ = x2, cy_ = y2) ;
    return *this ;
}

PathBuilder & PathBuilder::curveToRel(double x, double y, double x1, double y1, double x2, double y2) {
    addCommand(PathData::CurveToCmd,  cx_ + x, cy_ + y,
               rx_ = cx_ + x1, ry_ = cy_ + y1,
               cx_ + x2, cy_ + y2) ;
    cx_ += x2 ; cy_ += y2 ;
    return *this ;
}

PathBuilder & PathBuilder::quadTo(double arg1, double arg2, double arg3, double arg4) {
    rx_ = arg1 ; ry_ = arg2 ;

    /* raise quadratic bezier to cubic */
    double x1 = (cx_ + 2 * arg1) * (1.0 / 3.0);
    double y1 = (cy_ + 2 * arg2) * (1.0 / 3.0);
    double x3 = arg3 ;
    double y3 = arg4 ;
    double x2 = (x3 + 2 * arg1) * (1.0 / 3.0);
    double y2 = (y3 + 2 * arg2) * (1.0 / 3.0);

    addCommand(PathData::CurveToCmd, x1, y1, x2, y2, x3, y3) ;

    cx_ = arg3 ;
    cy_ = arg4 ;

    previous_cmd_ = PathData::QuadCurveToCmd ;

    return *this ;
}


PathBuilder & PathBuilder::quadToRel(double arg1, double arg2, double arg3, double arg4) {
    arg1 += cx_ ; arg3 += cx_ ;
    arg2 += cy_ ; arg4 += cy_ ;

    quadTo(arg1, arg2, arg3, arg4) ;

    return *this ;
}

PathBuilder & PathBuilder::smoothCurveTo(double arg3, double arg4, double arg5, double arg6)
{
    double arg1, arg2 ;

    if ( previous_cmd_ == PathData::CurveToCmd ) {
        arg1 = 2 * cx_ - rx_ ;
        arg2 = 2 * cy_ - ry_ ;
    }
    else {
        arg1 = cx_ ; arg2 = cy_ ;
    }

    addCommand(PathData::CurveToCmd,
               arg1, arg2, rx_ = arg3, ry_ = arg4, cx_ = arg5, cy_ = arg6) ;

    return *this ;

}

PathBuilder & PathBuilder::smoothCurveToRel(double arg3, double arg4, double arg5, double arg6) {
    double arg1, arg2 ;

    if ( previous_cmd_ == PathData::CurveToCmd ) {
        arg1 = 2 * cx_ - rx_ ;
        arg2 = 2 * cy_ - ry_ ;
    }
    else    {
        arg1 = cx_ ; arg2 = cy_ ;
    }

    arg1 -= cx_ ; arg2 -= cy_ ;

    addCommand(PathData::CurveToCmd,
               cx_ + arg1, cy_ + arg2, rx_ = cx_ + arg3, ry_ = cy_ + arg4,
               arg5 + cx_, arg6 + cy_) ;
    cx_ += arg5 ;
    cy_ += arg6 ;

    return *this ;
}

PathBuilder & PathBuilder::smoothQuadTo(double arg3, double arg4) {
    double arg1, arg2 ;

    if ( previous_cmd_ == PathData::QuadCurveToCmd ) {
        arg1 = 2 * cx_ - rx_ ;
        arg2 = 2 * cy_ - ry_ ;
    }
    else
    {
        arg1 = cx_ ; arg2 = cy_ ;
    }

    /* raise quadratic bezier to cubic */
    double x1 = (cx_ + 2 * arg1) * (1.0 / 3.0);
    double y1 = (cy_ + 2 * arg2) * (1.0 / 3.0);
    double x3 = arg3 ;
    double y3 = arg4 ;
    double x2 = (x3 + 2 * arg1) * (1.0 / 3.0);
    double y2 = (y3 + 2 * arg2) * (1.0 / 3.0);

    addCommand(PathData::CurveToCmd, x1, y1, x2, y2, x3, y3) ;

    cx_ = arg3 ;
    cy_ = arg4 ;

    previous_cmd_ = PathData::QuadCurveToCmd ;

    return *this ;
}

PathBuilder & PathBuilder::smoothQuadToRel(double arg3, double arg4) {
    double arg1, arg2 ;

    if ( previous_cmd_ == PathData::QuadCurveToCmd ) {
        arg1 = 2 * cx_ - rx_ ;
        arg2 = 2 * cy_ - ry_ ;
    }
    else {
        arg1 = cx_ ; arg2 = cy_ ;
    }

    arg3 += cx_ ; arg4 += cy_ ;

    /* raise quadratic bezier to cubic */
    double x1 = (cx_ + 2 * arg1) * (1.0 / 3.0);
    double y1 = (cy_ + 2 * arg2) * (1.0 / 3.0);
    double x3 = arg3 ;
    double y3 = arg4 ;
    double x2 = (x3 + 2 * arg1) * (1.0 / 3.0);
    double y2 = (y3 + 2 * arg2) * (1.0 / 3.0);

    addCommand(PathData::CurveToCmd, x1, y1, x2, y2, x3, y3) ;

    cx_ = arg3 ;
    cy_ = arg4 ;

    previous_cmd_ = PathData::QuadCurveToCmd ;

    return *this ;

}

PathBuilder & PathBuilder::arcTo(double arg1, double arg2, double arg3, bool arg4, bool arg5, double arg6, double arg7) {

    /**
            * rsvg_PathBuilder_arc: Add an RSVG arc to the PathBuilder context.
            * @ctx: PathBuilder context.
            * @rx: Radius in x direction (before rotation).
            * @ry: Radius in y direction (before rotation).
            * @x_axis_rotation: Rotation angle for axes.
            * @large_arc_flag: 0 for arc length <= 180, 1 for arc >= 180.
            * @sweep: 0 for "negative angle", 1 for "positive angle".
            * @x: New x coordinate.
            * @y: New y coordinate.
            *
            **/

    double rx = arg1 ;
    double ry = arg2 ;
    double x_axis_rotation = arg3 ;
    int large_arc_flag = (arg4 == false ) ? 0 : 1 ;
    int sweep_flag = (arg5 == false ) ? 0 : 1 ;
    double x = arg6 ;
    double y = arg7 ;

    double sin_th, cos_th;
    double a00, a01, a10, a11;
    double x0, y0, x1, y1, xc, yc;
    double d, sfactor, sfactor_sq;
    double th0, th1, th_arc;
    int i, n_segs;

    /* Check that neither radius is zero, since its isn't either
            geometrically or mathematically meaningful and will
            cause divide by zero and subsequent NaNs.  We should
            really do some ranged check ie -0.001 < x < 000.1 rather
            can just a straight check again zero.
            */
    if ((rx == 0.0) || (ry == 0.0)) return *this ;

    double cx = cx_, cy = cy_ ;
    sin_th = sin (x_axis_rotation * (M_PI / 180.0));
    cos_th = cos (x_axis_rotation * (M_PI / 180.0));
    a00 = cos_th / rx;
    a01 = sin_th / rx;
    a10 = -sin_th / ry;
    a11 = cos_th / ry;
    x0 = a00 * cx + a01 * cy;
    y0 = a10 * cx + a11 * cy;
    x1 = a00 * x + a01 * y;
    y1 = a10 * x + a11 * y;
    /* (x0, y0) is current point in transformed coordinate space.
            (x1, y1) is new point in transformed coordinate space.

            The arc fits a unit-radius circle in this space.
            */
    d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
    sfactor_sq = 1.0 / d - 0.25;
    if (sfactor_sq < 0) sfactor_sq = 0;
    sfactor = sqrt (sfactor_sq);
    if (sweep_flag == large_arc_flag)  sfactor = -sfactor;
    xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
    yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);
    /* (xc, yc) is center of the circle. */

    th0 = atan2 (y0 - yc, x0 - xc);
    th1 = atan2 (y1 - yc, x1 - xc);

    th_arc = th1 - th0;
    if (th_arc < 0 && sweep_flag) th_arc += 2 * M_PI;
    else if (th_arc > 0 && !sweep_flag) th_arc -= 2 * M_PI;

    n_segs = ceil (fabs (th_arc / (M_PI * 0.5 + 0.001)));

    for (i = 0; i < n_segs; i++)
    {
        double ccc[6] ;
        svg_path_arc_segment (ccc, xc, yc,
                              th0 + i * th_arc / n_segs,
                              th0 + (i + 1) * th_arc / n_segs, rx, ry, x_axis_rotation);

        addCommand(PathData::CurveToCmd,	ccc[0], ccc[1], ccc[2], ccc[3], ccc[4], ccc[5]) ;
    }

    cx_ = x ;
    cy_ = y ;

    return *this ;
}

PathBuilder & PathBuilder::arcToRel(double arg1, double arg2, double arg3, bool arg4, bool arg5, double arg6, double arg7) {

    arg6 += cx_ ; arg7 += cy_ ;

    arcTo(arg1, arg2, arg3, arg4, arg5, arg6, arg7) ;

    return *this ;
}

PathBuilder &PathBuilder::addEllipse(double x0, double y0, double r1, double r2) {
    moveTo(x0, y0-r2) ;
    arcTo(r1, r2, 0, true, true, x0, y0+r2) ;
    arcTo(r1, r2, 0, true, true, x0, y0-r2) ;
    closePath() ;
    return *this ;
}

PathBuilder &PathBuilder::addArc(double x0, double y0, double r1, double r2, double startAngle, double sweepAngle) {
    double sx = x0 + r1 * cos(startAngle * M_PI/180) ;
    double sy = y0 + r2 * sin(startAngle * M_PI/180) ;
    double ex = x0 + r1 * cos((startAngle + sweepAngle) * M_PI/180) ;
    double ey = y0 + r2 * sin((startAngle + sweepAngle) * M_PI/180) ;

    moveTo(sx, sy) ;
    arcTo(r1, r2, 0, (sweepAngle > 180 ) , true, ex, ey) ;

    return *this ;
}

PathBuilder &PathBuilder::addRect(double x0, double y0, double w, double h) {
    moveTo(x0, y0) ;
    lineTo(x0+w, y0) ;
    lineTo(x0+w, y0+h) ;
    lineTo(x0, y0+h) ;
    closePath() ;

    return *this ;
}

PathBuilder &PathBuilder::addRoundedRect(double xp, double yp, double wp, double hp, double rxp, double ryp) {

    if (rxp > fabs (wp / 2.))
        rxp = fabs (wp / 2.);
    if (ryp > fabs (hp / 2.))
        ryp = fabs (hp / 2.);

    if (rxp == 0) rxp = ryp;
    else if (ryp == 0) ryp = rxp ;

    if ( wp != 0.0 && hp != 0.0 )
    {
        if ( rxp == 0.0 || ryp == 0.0 )
            addRect(xp, yp, wp, hp) ;
        else
        {
            moveTo(xp + rxp, yp) ;
            lineTo(xp + wp - rxp, yp) ;
            arcTo(rxp, ryp, 0, false, true, xp + wp, yp + ryp) ;
            lineTo(xp + wp, yp + hp - ryp) ;
            arcTo(rxp, ryp, 0, false, true, xp + wp -rxp, yp + hp) ;
            lineTo(xp + rxp, yp + hp) ;
            arcTo(rxp, ryp, 0, false, true, xp, yp + hp - ryp) ;
            lineTo(xp, yp + ryp) ;
            arcTo(rxp, ryp, 0, false, true, xp + rxp, yp) ;
            closePath() ;
        }
    }


    return *this ;

}

PathBuilder & PathBuilder::addPath(const Path &other) {
    std::copy(other.data().commands().begin(), other.data().commands().end(),
              std::back_inserter(data_.cmds_)) ;

    return *this ;
}

PathBuilder &PathBuilder::addPolygon(const std::vector<Point2d> &pts) {
    assert (pts.size() > 2) ;

    moveTo(pts[0].x(), pts[0].y()) ;
    for( uint i=1 ; i<pts.size() ; i++ )
        lineTo(pts[i].x(), pts[i].y()) ;
    closePath() ;

    return *this ;
}

PathBuilder &PathBuilder::addPolyline(const std::vector<Point2d> &pts) {
    assert(pts.size() >= 2) ;

    moveTo(pts[0].x(), pts[0].y()) ;
    for( uint i=1 ; i<pts.size() ; i++ )
        lineTo(pts[i].x(), pts[i].y()) ;

    return *this ;

}

void PathBuilder::addCommand(PathData::Command cmd, double arg0, double arg1, double arg2, double arg3, double arg4, double arg5)
{
    previous_cmd_ = cmd ;
    data_.addCommand(cmd, arg0, arg1, arg2, arg3, arg4, arg5) ;
}

Path::Path() {
    data_.reset(new PathData) ;
}

PathBuilder Path::builder() {
    return PathBuilder(*data_) ;
}

Path Path::transformed(const Matrix2d &m) const {
    Path res ;

    for ( const PathData::CommandBlock &block: data_->commands() ) {

        switch ( block.cmd_ ) {
        case PathData::MoveToCmd:
        case PathData::LineToCmd: {
            auto v = m.transform(block.arg0_, block.arg1_) ;
            res.data_->cmds_.emplace_back(block.cmd_, v.x(), v.y()) ;
            break ;
        }
        case PathData::CurveToCmd: {
            auto v1 = m.transform(block.arg0_, block.arg1_) ;
            auto v2 = m.transform(block.arg2_, block.arg3_) ;
            auto v3 = m.transform(block.arg4_, block.arg5_) ;
            res.data_->cmds_.emplace_back(block.cmd_, v1.x(), v1.y(), v2.x(), v2.y(), v3.x(), v3.y()) ;
            break ;
        }
        case PathData::ClosePathCmd:
            res.data_->cmds_.emplace_back(block.cmd_) ;
            break ;
        }

    }

    return res ;
}

inline void extent_box(double &minx, double &miny, double &maxx, double &maxy, double px, double py) {
    minx = std::min(minx, px) ;
    miny = std::min(miny, py) ;
    maxx = std::max(maxx, px) ;
    maxy = std::max(maxy, py) ;
}

Rectangle2d Path::extents() const
{
    double minx = std::numeric_limits<double>::max() ;
    double miny = std::numeric_limits<double>::max() ;

    double maxx = -std::numeric_limits<double>::max() ;
    double maxy = -std::numeric_limits<double>::max() ;

    for ( const PathData::CommandBlock &block: data_->cmds_ ) {

        switch ( block.cmd_ ) {
        case PathData::MoveToCmd:
        case PathData::LineToCmd:
            extent_box(minx, miny, maxx, maxy, block.arg0_, block.arg1_) ;
            break ;
        case PathData::CurveToCmd:
            extent_box(minx, miny, maxx, maxy, block.arg0_, block.arg1_) ;
            extent_box(minx, miny, maxx, maxy, block.arg2_, block.arg3_) ;
            extent_box(minx, miny, maxx, maxy, block.arg4_, block.arg5_) ;
            break ;
        }
    }

    return Rectangle2d({minx, miny}, {maxx, maxy}) ;
}

msg::Path *Path::write(const Path &path) {
    msg::Path *msg = new msg::Path ;
    for( const PathData::CommandBlock &cmd: path.data().commands() ) {
        msg::PathCommand *msg_cmd = msg->add_commands() ;
        switch ( cmd.cmd_ ) {
        case PathData::MoveToCmd:
            msg_cmd->set_command(msg::PathCommand::CMD_MOVE_TO) ;
            msg_cmd->set_arg0(cmd.arg0_) ;
            msg_cmd->set_arg1(cmd.arg1_) ;
            break ;
        case PathData::LineToCmd:
            msg_cmd->set_command(msg::PathCommand::CMD_LINE_TO) ;
            msg_cmd->set_arg0(cmd.arg0_) ;
            msg_cmd->set_arg1(cmd.arg1_) ;
            break ;
        case PathData::CurveToCmd:
            msg_cmd->set_command(msg::PathCommand::CMD_CURVE_TO) ;
            msg_cmd->set_arg0(cmd.arg0_) ;
            msg_cmd->set_arg1(cmd.arg1_) ;
            msg_cmd->set_arg2(cmd.arg2_) ;
            msg_cmd->set_arg3(cmd.arg3_) ;
            msg_cmd->set_arg4(cmd.arg4_) ;
            msg_cmd->set_arg5(cmd.arg5_) ;
            break ;
        case PathData::ClosePathCmd:
            msg_cmd->set_command(msg::PathCommand::CMD_CLOSE) ;
        }
    }

    return msg ;
}

Path Path::read(const msg::Path &p){
    Path path ;

    auto builder = path.builder() ;

    for( const msg::PathCommand &cmd: p.commands() ) {
        switch ( cmd.command() ) {
        case msg::PathCommand::CMD_MOVE_TO:
            builder.moveTo(cmd.arg0(), cmd.arg1()) ;
            break ;
        case msg::PathCommand::CMD_LINE_TO:
            builder.lineTo(cmd.arg0(), cmd.arg1()) ;
            break ;
        case msg::PathCommand::CMD_CURVE_TO:
            builder.curveTo(cmd.arg0(), cmd.arg1(),
                  cmd.arg2(), cmd.arg3(), cmd.arg4(), cmd.arg5()) ;
            break ;
        case msg::PathCommand::CMD_CLOSE:
            builder.closePath() ;
            break ;
        }
    }

    return path ;
}



}
