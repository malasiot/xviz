#ifndef XVIZ_QT_WEBSOCKET_CLIENT_HPP
#define XVIZ_QT_WEBSOCKET_CLIENT_HPP

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>

#include <xviz/channel.hpp>

namespace xviz {
    namespace msg {
        class StateUpdate ;
    }
}

class WebSocketClient : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketClient(const QUrl &url, const QVector<QByteArray> &channels, bool debug = false, QObject *parent = nullptr);
    void connect() ;

Q_SIGNALS:
    void closed();
    void connected(const std::vector<xviz::Channel> &) ;
    void stateUpdated(const xviz::msg::StateUpdate &update) ;

private Q_SLOTS:
    void onConnected();
    void onTextMessageReceived(QString message);
    void onBinaryMessageReceived(QByteArray message);

private:
    QWebSocket socket_;
    QUrl url_;
    bool debug_;
    QVector<QByteArray> channels_  ;

};

#endif

