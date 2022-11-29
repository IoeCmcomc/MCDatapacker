#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QDir>
#include <QFontDatabase>

int main(int argc, char *argv[]) {
    qSetMessagePattern(
        "%{time}%{if-debug} (%{file}:%{line})%{endif} - %{type}%{if-category} - [%{category}]%{endif}: %{message}");

    QCoreApplication::setOrganizationName("IoeCmcomc");
    QCoreApplication::setApplicationName("MCDatapacker");
    QCoreApplication::setApplicationVersion("0.5.0");

    /*Q_INIT_RESOURCE(application); */

    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << QStringLiteral(":/icon"));
    QIcon::setFallbackThemeName(QStringLiteral("default"));
    if (QIcon::themeName().isEmpty())
        QIcon::setThemeName(QStringLiteral("default"));

    QApplication a(argc, argv);
    qInfo() << "Appication started.";

    const int fontId = QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/Monocraft/Monocraft.otf"));
    qDebug() << fontId;

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    /*parser.addPositionalArgument("file", "The file to open."); */
    parser.process(a);

    MainWindow w;
    w.show();
    qInfo() << "Appication startup completed.";

    return QApplication::exec();
}
