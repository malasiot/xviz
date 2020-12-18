#ifndef XVIZ_QT_RESOURCE_LOADER
#define XVIZ_QT_RESOURCE_LOADER

#include <QNetworkAccessManager>
#include <QFile>

class ResourceLoader: public QObject {
    Q_OBJECT
public:
    ResourceLoader() ;

    void fetch(const QString &path);

    static ResourceLoader &instance() {
        static ResourceLoader loader ;
        return loader ;
    }

    void setHost(const QString &host) {
        host_ = host ;
    }

    void setLocalPath(const QString &p) {
        local_path_ = p ;
    }

signals:

    void downloaded(const QByteArray &data, const QString &path) ;

private:

    QNetworkAccessManager *manager_ ;
    QString host_, local_path_ ;


private slots:
    void replyFinished(QNetworkReply* reply);
};







#endif
