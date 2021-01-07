#include "image_view.hpp"

#include <QToolBar>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>

ImageView::ImageView(): QMainWindow() {

    image_widget_ = new ImageWidget(this) ;
//    image_widget_->setMinimumSize(200, 200) ;

    QImage im ;
    im.load("/home/malasiot/Downloads/IMG_20201207_091133554.jpg");
    image_widget_->setImage(im) ;

    setCentralWidget(image_widget_);

    image_toolbar_ = new QToolBar("Image", this);
    addToolBar(image_toolbar_) ;

    createActions() ;
    addRectSelectionTool() ;
    addPolySelectionTool() ;


}

void ImageView::createActions()
{
    group_act_ = new QActionGroup(this) ;

    zoom_in_act_ = new QAction(QIcon(":/images/zoom-in.png"), tr("Zoom In"), this);
    zoom_in_act_->setShortcut(tr("Ctrl++"));
    zoom_in_act_->setStatusTip(tr("Zoom In"));
    connect(zoom_in_act_, SIGNAL(triggered()), this, SLOT(setTool()));
    zoom_in_act_->setCheckable(true);

    zoom_out_act_ = new QAction(QIcon(":/images/zoom-out.png"), tr("Zoom Out"), this);
    zoom_out_act_->setShortcut(tr("Ctrl+-"));
    zoom_out_act_->setStatusTip(tr("Zoom Out"));
    connect(zoom_out_act_, SIGNAL(triggered()), this, SLOT(setTool()));
    zoom_out_act_->setCheckable(true);

    zoom_fit_act_ = new QAction(QIcon(":/images/zoom-fit.png"), tr("Zoom to Fit"), this);
    zoom_fit_act_->setStatusTip(tr("Zoom to Fit"));
    connect(zoom_fit_act_, SIGNAL(triggered()), this, SLOT(zoomFit()));

    zoom_rect_act_ = new QAction(QIcon(":/images/zoom-rect.png"), tr("Zoom to Rectangle"), this);
    zoom_rect_act_->setStatusTip(tr("Zoom to Rectangle"));
    connect(zoom_rect_act_, SIGNAL(triggered()), this, SLOT(setTool()));
    zoom_rect_act_->setCheckable(true);

    pan_act_ = new QAction(QIcon(":/images/pan-tool.png"), tr("Pan view"), this);
    pan_act_->setStatusTip(tr("Pan image view"));
    connect(pan_act_, SIGNAL(triggered()), this, SLOT(setTool()));
    pan_act_->setCheckable(true) ;

    pan_act_->setChecked(true) ;

    group_act_->addAction(zoom_in_act_) ;
    group_act_->addAction(zoom_out_act_) ;
    group_act_->addAction(zoom_rect_act_) ;
    group_act_->addAction(zoom_fit_act_) ;
    group_act_->addAction(pan_act_) ;

    image_toolbar_->addAction(zoom_fit_act_) ;
    image_toolbar_->addSeparator() ;
    zoom_combo_ = new QComboBox(this) ;
    zoom_combo_->setFocusPolicy(Qt::ClickFocus) ;

    zoom_combo_->addItem("100%") ;
    zoom_combo_->addItem("To Fit") ;
    zoom_combo_->addItem("2%") ;
    zoom_combo_->addItem("5%") ;
    zoom_combo_->addItem("10%") ;
    zoom_combo_->addItem("25%") ;
    zoom_combo_->addItem("33%") ;
    zoom_combo_->addItem("50%") ;
    zoom_combo_->addItem("75%") ;
    zoom_combo_->addItem("100%") ;
    zoom_combo_->addItem("150%") ;
    zoom_combo_->addItem("x2") ;
    zoom_combo_->addItem("x3") ;
    zoom_combo_->addItem("x4") ;
    zoom_combo_->addItem("x5") ;
    zoom_combo_->addItem("x6") ;
    zoom_combo_->addItem("x7") ;
    zoom_combo_->addItem("x8") ;
    zoom_combo_->addItem("x9") ;
    zoom_combo_->addItem("x10") ;
    zoom_combo_->addItem("x11") ;
    zoom_combo_->addItem("x12") ;
    zoom_combo_->addItem("x13") ;
    zoom_combo_->addItem("x14") ;
    zoom_combo_->addItem("x15") ;
    zoom_combo_->addItem("x16") ;
    zoom_combo_->setMaxVisibleItems(zoom_combo_->count()) ;

    image_toolbar_->addWidget(zoom_combo_) ;
    image_toolbar_->connect(zoom_combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(setZoom()));

    image_toolbar_->addAction(zoom_in_act_) ;
    image_toolbar_->addAction(zoom_out_act_) ;
    image_toolbar_->addAction(zoom_rect_act_) ;

    image_toolbar_->addSeparator() ;

    image_toolbar_->addAction(pan_act_) ;

    ZoomInTool *zoom_in_tool = new ZoomInTool(image_widget_);
    connect(zoom_in_tool, &ZoomInTool::zoomIn, image_widget_, [this](const QPointF &p){
        image_widget_->zoomToPoint(p, +1) ;}) ;
    ZoomOutTool *zoom_out_tool = new ZoomOutTool(image_widget_);
    connect(zoom_out_tool, &ZoomOutTool::zoomOut, image_widget_, [this](const QPointF &p){
        image_widget_->zoomToPoint(p, -1) ;}) ;
    ZoomRectTool *zoom_rect_tool = new ZoomRectTool(image_widget_);
    connect(zoom_rect_tool, &ZoomRectTool::zoomToRect, image_widget_, &ImageWidget::zoomToRect);

    registerTool(pan_act_, new PanTool(image_widget_)) ;
    registerTool(zoom_in_act_, zoom_in_tool) ;
    registerTool(zoom_out_act_, zoom_out_tool) ;
    registerTool(zoom_rect_act_, zoom_rect_tool) ;
}


void ImageView::addRectSelectionTool()
{
    rect_tool_act_ = new QAction(QIcon(":/images/rect-tool.png"), tr("Rectangle Tool"), this);
    rect_tool_act_->setStatusTip(tr("Select a rectangular region"));
    connect(rect_tool_act_, SIGNAL(triggered()), this, SLOT(setTool()));
    rect_tool_act_->setCheckable(true) ;

    group_act_->addAction(rect_tool_act_) ;
    image_toolbar_->addAction(rect_tool_act_) ;

    RectTool *tool = new RectTool(image_widget_) ;
    registerTool(rect_tool_act_, tool) ;

    tool->setBrush(QBrush(Qt::NoBrush));

    QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(0, 0, 0, 0, nullptr);
    QPen epen(Qt::SolidLine);
    epen.setWidth(2) ;
    epen.setColor(Qt::blue) ;
    ellipse->setPen(epen) ;
    image_widget_->scene()->addItem(ellipse) ;
    connect(tool, &RectTool::rectChanged, this, [ellipse](const QRectF &r) { ellipse->setRect(r);});

}

void ImageView::addPolySelectionTool()
{
    poly_tool_act_ = new QAction(QIcon(":/images/polygon-tool.png"), tr("Polygon Tool"), this);
    poly_tool_act_->setStatusTip(tr("Select points"));
    connect(poly_tool_act_, SIGNAL(triggered()), this, SLOT(setTool()));
    poly_tool_act_->setCheckable(true) ;

    group_act_->addAction(poly_tool_act_) ;
    image_toolbar_->addAction(poly_tool_act_) ;

    PolygonTool *tool = new PolygonTool(image_widget_) ;
    registerTool(poly_tool_act_, tool) ;

    QGraphicsPolygonItem *polygon = new QGraphicsPolygonItem(QPolygonF(), nullptr);
    QPen epen(Qt::SolidLine);
    epen.setWidth(2) ;
    epen.setColor(Qt::red) ;
    QBrush ebrush(Qt::SolidPattern);
    ebrush.setColor(QColor(0, 0, 100, 100));
    polygon->setPen(epen) ;
    polygon->setBrush(ebrush) ;
    image_widget_->scene()->addItem(polygon) ;
    connect(tool, &PolygonTool::polygonChanged, this, [polygon](const QPolygonF &r) { polygon->setPolygon(r);});
}

void ImageView::registerTool(QAction *a, ImageTool *tool) {
    tools_.insert(a, tool) ;
}

void ImageView::fitToWindow() {
    image_widget_->zoomFit() ;
    updateZoomCombo(image_widget_->getZoom()) ;
}

void ImageView::fitToRect(const QRect &rect) {
    image_widget_->zoomToRect(rect) ;
    updateZoomCombo(image_widget_->getZoom()) ;
}


void ImageView::setTool() {
    ImageTool *tool = tools_.value(qobject_cast<QAction *>(sender()), nullptr) ;
    if ( tool ) image_widget_->setTool(tool) ;
}

void ImageView::updateZoomCombo(int idx)
{
    if ( idx == 7 ) zoom_combo_->setCurrentIndex(0) ;
    else zoom_combo_->setCurrentIndex(idx + 2) ;
}

void ImageView::setZoom()
{
    int idx = zoom_combo_->currentIndex() ;

    if ( idx == 0 ) image_widget_->setZoom(7) ;
    else if ( idx == 1 ) fitToWindow() ;
    else image_widget_->setZoom(idx-2) ;

}

void ImageView::zoomIn()
{
    image_widget_->zoomRel(1) ;

}

void ImageView::zoomOut()
{
    image_widget_->zoomRel(-1) ;
}

void ImageView::zoomRect()
{

}

void ImageView::zoomFit()
{
    fitToWindow() ;
}

