#include "resource_loader.hpp"

#include <QNetworkReply>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QDir>

ResourceLoader::ResourceLoader() {
    manager_ = new QNetworkAccessManager(this) ;
    QObject::connect(manager_, &QNetworkAccessManager::finished, this, &ResourceLoader::replyFinished);
}

void ResourceLoader::fetch(const QString &path) {
    if ( !local_path_.isEmpty() ) {
        QFileInfo fi(QDir(local_path_).filePath(path)) ;
        if ( fi.exists() ) {
            QFile file(fi.absoluteFilePath()) ;
            if ( file.open(QIODevice::ReadOnly) )
                emit downloaded(file.readAll(), path) ;
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

void ResourceLoader::replyFinished(QNetworkReply *reply) {
    emit downloaded(reply->readAll(), reply->request().url().path()) ;
}
