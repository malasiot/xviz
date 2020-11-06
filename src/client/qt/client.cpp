#include "client.hpp"
#include <QtCore/QDebug>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

#include <session.pb.h>
#include <xviz/channel.hpp>


using namespace std ;

QT_USE_NAMESPACE

WebSocketClient::WebSocketClient(const QUrl &url, const QVector<QByteArray> &channels, bool debug, QObject *parent) :
    QObject(parent), url_(url), debug_(debug), channels_(channels) {
    if ( debug_ ) qDebug() << "WebSocket server:" << url;
    QObject::connect(&socket_, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    QObject::connect(&socket_, &QWebSocket::disconnected, this, &WebSocketClient::closed);

}

void WebSocketClient::connect() {
     socket_.open(QUrl(url_));
}

static QByteArray makeSessionMessage(const QVector<QByteArray> &channels) {

    xviz::msg::Message msg ;

    xviz::msg::SessionStart *ss_msg = new xviz::msg::SessionStart() ;

    for( const auto &channel: channels )
        ss_msg->add_channels(channel) ;

    msg.set_allocated_session_start(ss_msg) ;

    std::string payload = msg.SerializeAsString() ;
    return QByteArray::fromStdString(payload) ;
}

void WebSocketClient::onConnected() {
    if ( debug_ ) qDebug() << "WebSocket connected";

    QObject::connect(&socket_, &QWebSocket::textMessageReceived,  this, &WebSocketClient::onTextMessageReceived);
    QObject::connect(&socket_, &QWebSocket::binaryMessageReceived, this, &WebSocketClient::onBinaryMessageReceived);

    socket_.sendBinaryMessage(makeSessionMessage(channels_));
}

void WebSocketClient::onTextMessageReceived(QString message) {
    if ( debug_ ) qDebug() << "Message received:" << message;
}

void WebSocketClient::onBinaryMessageReceived(QByteArray message) {
    using namespace xviz::msg ;
    Message msg_body ;

    if ( msg_body.ParseFromString(message.toStdString()) ) {
        switch ( msg_body.msg_case() ) {
        case Message::kSessionConfig: {
            vector<xviz::Channel> channelInfoList ;
            const SessionConfig &session_config_msg = msg_body.session_config() ;
            const auto &channels = session_config_msg.channel_info() ;
            for( const ChannelInfo &channel: channels ) {
                string name = channel.name() ;
                string desc = channel.description() ;
                int type = channel.type() ;

                channelInfoList.emplace_back(name, type, desc) ;

                qDebug() << name.c_str() << desc.c_str() << type ;
            }

             emit connected(channelInfoList) ;
        }
            break ;
        case Message::kStateUpdate: {
            const StateUpdate &state_update = msg_body.state_update() ;
            emit stateUpdated(state_update) ;
            break ;
            /*
            string channel_id = state_update.channel_id();
            string object_id = state_update.object_id() ;

            switch ( state_update.data_case() ) {
                case StateUpdate::kImageData:
                    const ImageData &image_data = state_update.image_data();

                    switch ( image_data.data_case() ) {
                    case ImageData::kRaw:
                        break ;
                    case ImageData::kUrl:
                        string url = image_data.url() ;

                        qDebug() << url.c_str() ;
                        break ;
                    }


            }
            */
        }
        break ;
        }

    }
    if (debug_)
        qDebug() << "Message received:" << message;
    //m_webSocket.close();
}
//! [onTextMessageReceived]
