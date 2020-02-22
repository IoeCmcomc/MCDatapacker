#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTranslator>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(application);

    qDebug() << "Starting application...";
    QApplication a(argc, argv);
    //a.setStyle("fusion");
    //a.setStyle("windowsxp");
    //a.setStyle("macintosh");

    QCoreApplication::setOrganizationName("IoeCmcomc");
    QCoreApplication::setApplicationName("Minecraft Datapack Maker");
    QCoreApplication::setApplicationVersion("beta-0.1.1");
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    //parser.addPositionalArgument("file", "The file to open.");
    parser.process(a);

    /*
    QTranslator qtTranslator;
    qtTranslator.load(QLocale::system(), QStringLiteral("qtbase_"));
    a.installTranslator(&qtTranslator);
    */

//    QTranslator translator;
//    translator.load("translations/MinecraftDatapackMaker_" + QLocale::system().name());
//    a.installTranslator(&translator);

    MainWindow w;
    //w.resize(250, 150);
    //w.setWindowTitle("Simple example");
    w.show();
    //w.showMaximized();
    //qDebug() << "Exitting application...";
    qDebug() << QLocale::system().name();

    return a.exec();
}
