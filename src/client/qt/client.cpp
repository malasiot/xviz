#include "client.hpp"
#include <QtCore/QDebug>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>


#include <session.pb.h>

QT_USE_NAMESPACE

//! [constructor]
EchoClient::EchoClient(const QUrl &url, const QVector<QByteArray> &channels, bool debug, QObject *parent) :
    QObject(parent),
    m_url(url),
    m_debug(debug),
  channels_(channels)
{
    if (m_debug)
        qDebug() << "WebSocket server:" << url;
    connect(&m_webSocket, &QWebSocket::connected, this, &EchoClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &EchoClient::closed);
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
void EchoClient::onConnected()
{
    if (m_debug)
        qDebug() << "WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &EchoClient::onTextMessageReceived);


    m_webSocket.sendBinaryMessage(makeSessionMessage(version_, format_, channels_));
}
//! [onConnected]

//! [onTextMessageReceived]
void EchoClient::onTextMessageReceived(QString message)
{
    if (m_debug)
        qDebug() << "Message received:" << message;
    m_webSocket.close();
}
//! [onTextMessageReceived]
