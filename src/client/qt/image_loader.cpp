#include "image_loader.hpp"

#include <QNetworkReply>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QDir>

ImageLoader::ImageLoader() {
    manager_ = new QNetworkAccessManager(this) ;
    QObject::connect(manager_, &QNetworkAccessManager::finished, this, &ImageLoader::replyFinished);
}

void ImageLoader::fetch(const QString &path) {
    if ( !local_path_.isEmpty() ) {
        QFileInfo fi(QDir(local_path_).filePath(path)) ;
        if ( fi.exists() ) {
            QImage im ;
            im.load(fi.absoluteFilePath()) ;
            emit downloaded(im) ;
        }
    } else {
        QUrl url ;
        url.setHost(host_) ;
        url.setPath(path) ;
        QNetworkRequest request;
        request.setUrl(url);
        manager_->get(request);
    }
}

void ImageLoader::replyFinished(QNetworkReply *reply)
{
    QImage image ;
    image.load(reply, nullptr);

    emit downloaded(image) ;
}
