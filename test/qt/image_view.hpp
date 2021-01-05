#pragma once

#include <QMainWindow>

#include <xviz/qt/image_widget.hpp>
#include <xviz/qt/image_toolbar.hpp>

class ImageView: public QMainWindow {
public:
    ImageView() ;

private:

    ImageToolBar *image_toolbar_ ;
    ImageWidget *image_widget_ ;
};
