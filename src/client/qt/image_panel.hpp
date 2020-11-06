#ifndef XVIZ_QT_IMAGE_PANEL_HPP
#define XVIZ_QT_IMAGE_PANEL_HPP

#include "panel.hpp"
#include "panel_config.hpp"
#include "image_widget.hpp"

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVBoxLayout>

class ImagePanel: public Panel {
    Q_OBJECT
public:
    ImagePanel(const ImagePanelConfig &config, QWidget *parent) ;

    void loadImageFromUrl(const QUrl &url) ;
    void displayImage(const QImage &im) ;

    void config(const std::vector<xviz::Channel> &channels) override ;
    void updateState(const xviz::msg::StateUpdate &) override ;
private:

    ImagePanelConfig config_ ;
    QNetworkAccessManager *manager_;
    ImageWidget *widget_ ;
    QVBoxLayout *layout_ ;

private slots:
    void replyFinished(QNetworkReply* reply);
};

#endif
