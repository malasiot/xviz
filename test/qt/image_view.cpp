#include "image_view.hpp"

ImageView::ImageView(): QMainWindow() {

    image_widget_ = new ImageWidget(this) ;
    image_widget_->setMinimumSize(200, 200) ;

    setCentralWidget(image_widget_);

    image_toolbar_ = new ImageToolBar(image_widget_, this) ;
    addToolBar(image_toolbar_) ;

    image_toolbar_->addZoomPanActions() ;

}
