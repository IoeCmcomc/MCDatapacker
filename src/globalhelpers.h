#ifndef GLOBALHELPERS_H
#define GLOBALHELPERS_H

#include "codefile.h"

#include <QString>
#include <QVariant>
#include <QDir>

namespace Glhp {
    bool isPathRelativeTo(const QString &dirpath, const QString &path,
                          const QString &catDir);
    QString randStr(int length = 5);
    QString relPath(const QString &dirpath, QString path);
    QString relNamespace(const QString &dirpath, QString path);

    CodeFile::FileType pathToFileType(const QString &dirpath,
                                      const QString &filepath);
    QIcon fileTypeToIcon(const CodeFile::FileType type);

    QString toNamespacedID(const QString &dirpath, QString filepath,
                           bool noTagForm = false);
    QVariant strToVariant(const QString &str);
    QString variantToStr(const QVariant &vari);
    QVector<QString> fileIdList(const QString &dirpath,
                                const QString &catDir = QString(),
                                const QString &nspace = QString(),
                                bool noTagForm        = true);

    bool removePrefix(QString &str, const QString &prefix);
}

#endif /* GLOBALHELPERS_H */
