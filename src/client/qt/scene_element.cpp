#include "scene_element.hpp"

#include <session.pb.h>

using namespace std ;

SceneElement::SceneElement() {

}

void SceneElement::buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) {

    widget_ = new SceneViewer(parent) ;

    widget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setup_flex(ele, widget_) ;
}


void SceneElement::updateState(const xviz::msg::StateUpdate &state_update) {
    string channel_id = state_update.channel_id();

    QByteArray channel = QByteArray::fromStdString(channel_id) ;
    if ( scene_channel_ == channel ) {


    }
    else if ( pose_channel_ == channel ) {

    }
    /*
    string data = state_update.data() ;

    xviz::msg::Image im ;
    if ( !im.ParseFromString(data) ) return ;

    xviz::Image image = xviz::Image::read(im) ;

    if ( image.type() == xviz::ImageType::Uri )  {
        QUrl imageUrl(QByteArray::fromStdString(image.uri())) ;
        loadImageFromUrl(imageUrl) ;
    } else if ( image.type() == xviz::ImageType::Raw ) {

    }
*/
}
