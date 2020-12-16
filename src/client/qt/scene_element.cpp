#include "scene_element.hpp"
#include "resource_loader.hpp"

#include <xviz/scene/scene.hpp>

#include <session.pb.h>
#include "scene.pb.h"

#include <QTemporaryFile>

using namespace std ;

SceneElement::SceneElement() {

}

void SceneElement::buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) {

    widget_ = new QWidget(parent) ;
    layout_ = new QVBoxLayout(widget_) ;
    widget_->setLayout(layout_) ;

    widget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setup_flex(ele, widget_) ;

    scene_channel_ = ele.attribute("model").toUtf8() ;
    pose_channel_ = ele.attribute("poses").toUtf8() ;
}


void SceneElement::updateState(const xviz::msg::StateUpdate &state_update) {
    string channel_id = state_update.channel_id();

    QByteArray channel = QByteArray::fromStdString(channel_id) ;
    if ( scene_channel_ == channel ) {

        string data = state_update.data() ;

        std::unique_ptr<xviz::SceneMessage> msg(xviz::SceneMessage::decode(data)) ;
        if ( msg->type() == xviz::SceneMessageType::Url ) {
            loadScene(msg->url())  ;
        } else if ( msg->type() == xviz::SceneMessageType::Data ) {

            viewer_ = new SceneViewer(widget_, msg->scene()) ;
            layout_->addWidget(viewer_) ;
        }

    }
    else if ( pose_channel_ == channel ) {

    }

}

void SceneElement::loadScene(const string &url)
{
    auto &loader = ResourceLoader::instance() ;

    QObject::connect(&loader, &ResourceLoader::downloaded, this, [&](const QByteArray &data, const QString &path){
        QString templateStr = "XXXXXX" ;
        int idx = path.lastIndexOf('.') ;
        if ( idx >= 0 ) templateStr.append(path.mid(idx));

        QTemporaryFile file(templateStr) ;
        if ( file.open() ) {
            file.write(data) ;
            file.close() ;
            xviz::ScenePtr scene(new xviz::Scene) ;
            scene->load(file.fileName().toStdString()) ;

            viewer_ = new SceneViewer(widget_, scene) ;
            layout_->addWidget(viewer_) ;
        }
    });

    loader.fetch(QString::fromStdString(url));
}
