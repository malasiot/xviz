#include "client.hpp"
#include <QtCore/QDebug>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

#include <session.pb.h>


using namespace std ;

QT_USE_NAMESPACE


//! [constructor]
Client::Client(const QUrl &url, const QVector<QByteArray> &channels, bool debug, QObject *parent) :
    QObject(parent),
    m_url(url),
    m_debug(debug),
  channels_(channels)
{
    if (m_debug)
        qDebug() << "WebSocket server:" << url;
    connect(&m_webSocket, &QWebSocket::connected, this, &Client::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &Client::closed);
    m_webSocket.open(QUrl(url));
}
//! [constructor]


static QByteArray makeSessionMessage(const QByteArray &version, const QByteArray &format,
                         const QVector<QByteArray> &channels) {

    xviz::msg::Message msg ;

    xviz::msg::SessionStart *ss_msg = new xviz::msg::SessionStart() ;

    for( const auto &channel: channels )
        ss_msg->add_channels(channel) ;

    msg.set_allocated_session_start(ss_msg) ;

    std::string payload = msg.SerializeAsString() ;
    return QByteArray::fromStdString(payload) ;

}
//! [onConnected]
void Client::onConnected()
{
    if (m_debug)
        qDebug() << "WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &Client::onTextMessageReceived);
    connect(&m_webSocket, &QWebSocket::binaryMessageReceived,
            this, &Client::onBinaryMessageReceived);

    m_webSocket.sendBinaryMessage(makeSessionMessage(version_, format_, channels_));


}
//! [onConnected]

//! [onTextMessageReceived]
void Client::onTextMessageReceived(QString message)
{
    if (m_debug)
        qDebug() << "Message received:" << message;
    m_webSocket.close();
}

void Client::onBinaryMessageReceived(QByteArray message)
{
    using namespace xviz::msg ;
    Message msg_body ;
    if ( msg_body.ParseFromString(message.toStdString()) ) {
        switch ( msg_body.msg_case() ) {
        case Message::kSessionConfig: {
            const SessionConfig &session_config_msg = msg_body.session_config() ;
            const auto &channels = session_config_msg.channel_info() ;
            for( const ChannelInfo &channel: channels ) {
                string name = channel.name() ;
                string desc = channel.description() ;
                int type = channel.type() ;

                qDebug() << name.c_str() << desc.c_str() << type ;
            }

             emit connected() ;
        }
            break ;
        case Message::kStateUpdate: {
            const StateUpdate &state_update = msg_body.state_update() ;
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
        }
        break ;
        }

    }
    if (m_debug)
        qDebug() << "Message received:" << message;
    //m_webSocket.close();
}
//! [onTextMessageReceived]
