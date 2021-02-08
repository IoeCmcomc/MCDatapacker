#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTranslator>
#include <QDebug>
#include <QDir>
#include <QStyleFactory>

int main(int argc, char *argv[]) {
    /*Q_INIT_RESOURCE(application); */

    /*qDebug() << "Starting application..."; */
    QApplication a(argc, argv);

    /*qDebug() << QStyleFactory::keys(); */
    /*
       a.setStyle("windowsvista");
       a.setStyle("Windows");
        a.setStyle("Fusion");
     */

    QCoreApplication::setOrganizationName("IoeCmcomc");
    QCoreApplication::setApplicationName("MCDatapacker");
    QCoreApplication::setApplicationVersion("0.3.0-beta");
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    /*parser.addPositionalArgument("file", "The file to open."); */
    parser.process(a);

    MainWindow w;
    w.show();

    return QApplication::exec();
}
