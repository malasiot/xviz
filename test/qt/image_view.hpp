#pragma once

#include <QMainWindow>
#include <QComboBox>

#include <xviz/qt/image_widget.hpp>
#include <xviz/qt/image_tools.hpp>

class ImageView: public QMainWindow {
    Q_OBJECT
public:
    ImageView() ;


private slots:
    void updateZoomCombo(int idx) ;

    void setZoom() ;
    void zoomIn() ;
    void zoomOut() ;
    void zoomRect() ;
    void zoomFit() ;
    void setTool() ;
private:

    void createActions() ;

    void addRectSelectionTool();
    void addPolySelectionTool();

    QToolBar *image_toolbar_ ;
    ImageWidget *image_widget_ ;

    QAction *zoom_in_act_, *zoom_out_act_, *zoom_fit_act_, *zoom_rect_act_, *pan_act_ ;
    QAction *rect_tool_act_, *poly_tool_act_ ;
    QActionGroup *group_act_ ;
    QComboBox *zoom_combo_ ;

    QMap<QAction *, ImageTool *> tools_ ;

    void fitToWindow();
    void fitToRect(const QRect &rect);
    void registerTool(QAction *a, ImageTool *tool);
};
