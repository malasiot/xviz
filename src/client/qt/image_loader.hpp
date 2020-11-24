#ifndef XVIZ_QT_IMAGE_LOADER
#define XVIZ_QT_IMAGE_LOADER

#include <QNetworkAccessManager>

class ImageLoader: public QObject {
    Q_OBJECT
public:
    ImageLoader() ;

    void fetch(const QString &path);

    static ImageLoader &instance() {
        static ImageLoader loader ;
        return loader ;
    }

    void setHost(const QString &host) {
        host_ = host ;
    }

    void setLocalPath(const QString &p) {
        local_path_ = p ;
    }

signals:

    void downloaded(QImage) ;

private:

    QNetworkAccessManager *manager_ ;
    QString host_, local_path_ ;


private slots:
    void replyFinished(QNetworkReply* reply);
};







#endif
