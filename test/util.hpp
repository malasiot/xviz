#pragma once

#include <QString>
#include <QFileInfo>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>

class TestApplication: public QApplication {
    Q_OBJECT
public:

    TestApplication(const QString &name, int &argc, char **argv);
    virtual ~TestApplication() {}

    static std::string data() { return s_data_dir_.toStdString() ; }

private:
    static QString s_data_dir_ ;

};
