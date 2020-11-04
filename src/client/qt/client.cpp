#include "client.hpp"
#include <QtCore/QDebug>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

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
    QJsonObject obj ;

    obj["message"] = "session" ;
    obj["version"] = QString(version) ;
    obj["format"] = QString(format) ;

    QJsonArray channelArray ;
    for( const auto &channel: channels )
        channelArray.append(QString(channel)) ;

    obj["channels"] = channelArray ;

    QJsonDocument doc(obj) ;

    return doc.toJson() ;

}
//! [onConnected]
void EchoClient::onConnected()
{
    if (m_debug)
        qDebug() << "WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &EchoClient::onTextMessageReceived);


    m_webSocket.sendTextMessage(makeSessionMessage(version_, format_, channels_));
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
