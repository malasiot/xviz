#include "image_panel.hpp"
#include <xviz/image.hpp>

#include <session.pb.h>
using namespace std ;

ImagePanel::ImagePanel(const ImagePanelConfig &config, QWidget *parent): config_(config), Panel(parent) {
    manager_ = new QNetworkAccessManager(this);
    QObject::connect(manager_, &QNetworkAccessManager::finished, this, &ImagePanel::replyFinished);

    widget_ = new ImageWidget(this) ;
    layout_ = new QVBoxLayout(this) ;
    layout_->addWidget(widget_) ;
    widget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setLayout(layout_) ;
}

void ImagePanel::loadImageFromUrl(const QUrl &url) {
    QNetworkRequest request;
    request.setUrl(url);
    manager_->get(request);
}

void ImagePanel::displayImage(const QImage &im) {
    widget_->setImage(im) ;
    widget_->zoomToRect(im.rect()) ;
}

void ImagePanel::config(const std::vector<xviz::Channel> &channels)
{

}

void ImagePanel::updateState(const xviz::msg::StateUpdate &state_update) {
    string channel_id = state_update.channel_id();
    //string object_id = state_update.object_id() ;
    if ( !config_.channels_.contains(QByteArray::fromStdString(channel_id))) return ;
    string data = state_update.data() ;

    xviz::msg::Image im ;
    if ( !im.ParseFromString(data) ) return ;

    std::unique_ptr<xviz::Image> image(xviz::Image::read(im)) ;

    if ( xviz::ImageUri *im = dynamic_cast<xviz::ImageUri *>(image.get()) ) {
        QUrl imageUrl(QByteArray::fromStdString(im->uri_)) ;
        loadImageFromUrl(imageUrl) ;
    } else if ( xviz::RawImage *im = dynamic_cast<xviz::RawImage *>(image.get()) ) {

    }

}

void ImagePanel::replyFinished(QNetworkReply *reply) {
    QImage image ;
    image.load(reply, nullptr);

    displayImage(image) ;
}
