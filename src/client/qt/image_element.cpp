#include "image_element.hpp"
#include <xviz/image.hpp>

#include <session.pb.h>

using namespace std ;

ImageElement::ImageElement() {

}

void ImageElement::buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) {

    image_widget_ = new ImageWidget(parent) ;

    manager_ = new QNetworkAccessManager(parent);
    QObject::connect(manager_, &QNetworkAccessManager::finished, this, &ImageElement::replyFinished);

  //  layout_ = new QVBoxLayout(this) ;
 //   layout_->addWidget(widget_) ;
    image_widget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   // setLayout(layout_) ;

    parseChannels(ele, channels_) ;

    setup_flex(ele, image_widget_) ;
}

void ImageElement::loadImageFromUrl(const QUrl &url) {
    QNetworkRequest request;
    request.setUrl(url);
    manager_->get(request);
}

void ImageElement::displayImage(const QImage &im) {
    image_widget_->setImage(im) ;
    image_widget_->zoomToRect(im.rect()) ;
}


void ImageElement::config(const std::vector<xviz::Channel> &channels)
{

}

void ImageElement::updateState(const xviz::msg::StateUpdate &state_update) {
    string channel_id = state_update.channel_id();
    //string object_id = state_update.object_id() ;
    if ( !channels_.contains(QByteArray::fromStdString(channel_id))) return ;
    string data = state_update.data() ;

    xviz::msg::Image im ;
    if ( !im.ParseFromString(data) ) return ;

    xviz::Image image = xviz::Image::read(im) ;

    if ( image.type() == xviz::ImageType::Uri )  {
        QUrl imageUrl(QByteArray::fromStdString(image.uri())) ;
        loadImageFromUrl(imageUrl) ;
    } else if ( image.type() == xviz::ImageType::Raw ) {

    }

}

void ImageElement::replyFinished(QNetworkReply *reply) {
    QImage image ;
    image.load(reply, nullptr);

    displayImage(image) ;
}
