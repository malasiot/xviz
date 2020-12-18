#ifndef XVIZ_QT_IMAGE_ELEMENT_HPP
#define XVIZ_QT_IMAGE_ELEMENT_HPP

#include "ui_element_factory.hpp"
#include <xviz/qt/image_widget.hpp>

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVBoxLayout>

class ImageElement: public UIElement {
    Q_OBJECT
public:
    ImageElement() ;

    void buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) override ;

    void loadImageFromUrl(const QUrl &url) ;
    void displayImage(const QImage &im) ;


    void getChannels(QVector<QByteArray> &channels) const override {
        channels.append(channels_) ;
    }

    void updateState(const xviz::msg::StateUpdate &) override ;

    QWidget *widget() const override { return image_widget_ ; }

private:


    QNetworkAccessManager *manager_;
    ImageWidget *image_widget_ ;
    QVector<QByteArray> channels_ ;

private slots:
    void replyFinished(QNetworkReply* reply);
};

#endif
