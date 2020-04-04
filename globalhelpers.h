#ifndef GLOBALHELPERS_H
#define GLOBALHELPERS_H

#include "mainwindow.h"

#include <QString>
#include <QVariant>

namespace GlobalHelpers {
    void someTest();
    bool isPathRelativeTo(const QString &dirpath, const QString &path,
                          const QString &catDir);
    QString randStr(int length = 5);
    QString relPath(const QString &dirpath, QString path);
    QString relNamespace(const QString &dirpath, QString path);
    MainWindow::MCRFileType toMCRFileType(const QString &dirpath,
                                          const QString &filepath);
    QString toNamespacedID(const QString &dirpath, QString filepath);
    QVariant strToVariant(const QString &str);
    QString variantToStr(const QVariant &vari);
}

#endif /* GLOBALHELPERS_H */
