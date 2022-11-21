#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTranslator>
#include <QDebug>
#include <QDir>
#include <QStyleFactory>

int main(int argc, char *argv[]) {
    qSetMessagePattern(
        "%{time}%{if-debug} (%{file}:%{line})%{endif} - %{type}%{if-category} - [%{category}]%{endif}: %{message}");

    QCoreApplication::setOrganizationName("IoeCmcomc");
    QCoreApplication::setApplicationName("MCDatapacker");
    QCoreApplication::setApplicationVersion("0.5.0");

    /*Q_INIT_RESOURCE(application); */

    QApplication a(argc, argv);
    qInfo() << "Appication started.";

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    /*parser.addPositionalArgument("file", "The file to open."); */
    parser.process(a);

    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/icon");
    qDebug() << QIcon::fallbackSearchPaths();
    qDebug() << QIcon::themeSearchPaths();

    MainWindow w;
    w.show();
    qInfo() << "Appication startup completed.";

    return QApplication::exec();
}
