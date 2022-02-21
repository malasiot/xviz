#include "util.hpp"

QString TestApplication::s_data_dir_ ;

TestApplication::TestApplication(const QString &name, int &argc, char **argv): QApplication(argc, argv) {

    setOrganizationDomain("iti.gr");
    setApplicationName(name);

    QStringList args = arguments() ;

    for(  int i=1 ; i<args.size() ; i++ ) {
        if ( args[i] == "--data" ) {
            QString data_dir = argv[i+1] ;

            QFileInfo fi(data_dir) ;

            if ( fi.exists()) {
                s_data_dir_ = fi.absoluteFilePath() ;
                return ;
            }
        }
    }


    QString app_path = applicationDirPath() ;
    QDir native_app_path(app_path) ;

    // detect if running from the source tree

    bool run_from_source = false ;

    do {
        if ( native_app_path.dirName().startsWith("build") ) {
            run_from_source = true ;
            break ;
        }
    } while ( native_app_path.cdUp() );



    QDir p(app_path + "/data/") ;
    if ( p.exists() ) {
        s_data_dir_  = p.canonicalPath() ;
        return ;
    }

    // default location

    s_data_dir_ = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
}
