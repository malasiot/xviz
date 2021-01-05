#include <xviz/qt/image_toolbar.hpp>
#include <xviz/qt/image_widget.hpp>

#include <QComboBox>
#include <QDirIterator>
#include <QDebug>

ImageToolBar::ImageToolBar(ImageWidget *w, QWidget *parent): QToolBar("Image", parent), image_widget_(w) {
    Q_INIT_RESOURCE(image_toolbar);
}

void ImageToolBar::addZoomPanActions()
{

    QDirIterator it(":", QDirIterator::Subdirectories);
    while (it.hasNext())
        qDebug() << it.next();

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

    pan_zoom_group_act_ = new QActionGroup(this) ;

    pan_zoom_group_act_->addAction(zoom_in_act_) ;
    pan_zoom_group_act_->addAction(zoom_out_act_) ;
    pan_zoom_group_act_->addAction(zoom_rect_act_) ;
    pan_zoom_group_act_->addAction(zoom_fit_act_) ;
    pan_zoom_group_act_->addAction(pan_act_) ;

    addAction(zoom_fit_act_) ;
    addSeparator() ;
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

    addWidget(zoom_combo_) ;
    connect(zoom_combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(setZoom()));

    addAction(zoom_in_act_) ;
    addAction(zoom_out_act_) ;
    addAction(zoom_rect_act_) ;

    addSeparator() ;

    addAction(pan_act_) ;



}



void ImageToolBar::fitToWindow() {
    image_widget_->zoomFit() ;
    updateZoomCombo(image_widget_->getZoom()) ;
}

void ImageToolBar::fitToRect(const QRect &rect) {
    image_widget_->zoomToRect(rect) ;
    updateZoomCombo(image_widget_->getZoom()) ;
}


void ImageToolBar::setTool()
{
//    QImageTool *tool = tools.value(qobject_cast<QAction *>(sender()), nullptr) ;

//    if ( tool ) image_widget_->setTool(tool) ;
}

void ImageToolBar::updateZoomCombo(int idx)
{
    if ( idx == 7 ) zoom_combo_->setCurrentIndex(0) ;
    else zoom_combo_->setCurrentIndex(idx + 2) ;
}

void ImageToolBar::setZoom()
{
    int idx = zoom_combo_->currentIndex() ;

    if ( idx == 0 ) image_widget_->setZoom(7) ;
    else if ( idx == 1 ) fitToWindow() ;
    else image_widget_->setZoom(idx-2) ;

}

void ImageToolBar::zoomIn()
{
    image_widget_->zoomRel(1) ;

}

void ImageToolBar::zoomOut()
{
    image_widget_->zoomRel(-1) ;
}

void ImageToolBar::zoomRect()
{

}

void ImageToolBar::zoomFit()
{
    fitToWindow() ;
}
