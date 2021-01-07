#include "image_view.hpp"

#include <QApplication>

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    ImageView window ;
    window.resize(1024, 1024) ;
    window.show() ;

    return app.exec();

}
