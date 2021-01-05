#ifndef XVIZ_QT_IMAGE_TOOLBAR_HPP
#define XVIZ_QT_IMAGE_TOOLBAR_HPP

#include <QToolBar>

class ImageWidget ;
class QComboBox ;

class ImageToolBar: public QToolBar {
    Q_OBJECT
public:

    ImageToolBar(ImageWidget *w, QWidget *parent) ;

    void addZoomPanActions() ;

protected slots:

    void updateZoomCombo(int idx) ;

    void setZoom() ;
    void zoomIn() ;
    void zoomOut() ;
    void zoomRect() ;
    void zoomFit() ;
    void setTool() ;

private:

    ImageWidget *image_widget_ ;
    QAction *zoom_in_act_, *zoom_out_act_, *zoom_fit_act_, *zoom_rect_act_, *pan_act_ ;
    QActionGroup *pan_zoom_group_act_ ;
    QComboBox *zoom_combo_ ;

    void fitToWindow();
    void fitToRect(const QRect &rect);
};

#endif
