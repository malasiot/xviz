#ifndef XVIZ_QT_SCENE_ELEMENT_HPP
#define XVIZ_QT_SCENE_ELEMENT_HPP

#include "ui_element_factory.hpp"
#include <xviz/qt/scene/viewer.hpp>

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVBoxLayout>

class SceneElement: public UIElement {
    Q_OBJECT
public:
    SceneElement() ;

    void buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) override ;

    void getChannels(QVector<QByteArray> &channels) const override {
        if ( !scene_channel_.isEmpty() ) channels.append(scene_channel_) ;
        if ( !pose_channel_.isEmpty() ) channels.append(pose_channel_) ;
    }

    void updateState(const xviz::msg::StateUpdate &) override ;

    QWidget *widget() const override { return widget_ ; }

private:

    void loadScene(const std::string &url) ;

    QWidget *widget_ ;
    QVBoxLayout *layout_ ;
    SceneViewer *viewer_ ;
    QByteArray scene_channel_, pose_channel_ ;
};

#endif
