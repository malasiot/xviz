#ifndef ECHOCLIENT_H
#define ECHOCLIENT_H

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(const QUrl &url, const QVector<QByteArray> &channels, bool debug = false, QObject *parent = nullptr);


Q_SIGNALS:
    void closed();
    void connected() ;

private Q_SLOTS:
    void onConnected();
    void onTextMessageReceived(QString message);
     void onBinaryMessageReceived(QByteArray message);

private:
    QWebSocket m_webSocket;
    QUrl m_url;
    bool m_debug;
    QByteArray version_ = "1.0", format_ = "JSON" ;
    QVector<QByteArray> channels_  ;

};

#endif // ECHOCLIENT_H

