#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTranslator>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[]) {
    /*Q_INIT_RESOURCE(application); */

    qDebug() << "Starting application...";
    QApplication a(argc, argv);
    /*
       a.setStyle("fusion");
       a.setStyle("windowsxp");
       a.setStyle("macintosh");
     */

    QCoreApplication::setOrganizationName("IoeCmcomc");
    QCoreApplication::setApplicationName("MCDatapacker");
    QCoreApplication::setApplicationVersion("beta-0.2.2");
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    /*parser.addPositionalArgument("file", "The file to open."); */
    parser.process(a);

    MainWindow w;
    /*
       w.resize(250, 150);
       w.setWindowTitle("Simple example");
     */
    w.show();
    /*
       w.showMaximized();
       qDebug() << "Exitting application...";
     */

    return a.exec();
}
