#include "mainwindow.h"

#include <QApplication>
#include <QFontDatabase>
#include <QWidget>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //a.setStyle("fusion");
    //a.setStyle("windowsxp");
    //a.setStyle("macintosh");

    MainWindow w;
    //w.resize(250, 150);
    //w.setWindowTitle("Simple example");
    //w.show();
    w.showMaximized();

    return a.exec();
}
