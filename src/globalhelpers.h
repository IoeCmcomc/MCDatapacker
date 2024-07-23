#ifndef GLOBALHELPERS_H
#define GLOBALHELPERS_H

#include <QString>
#include <QVariant>
#include <QDir>
#include <QIcon>

#ifndef QLATIN1STRING_OPERATOR
QLatin1String constexpr operator ""_QL1(const char *literal, size_t size) {
    return QLatin1String(literal, size);
}
#define QLATIN1STRING_OPERATOR
#endif

namespace Glhp {
    bool isPathRelativeTo(const QString &dirpath, QStringView path,
                          QStringView category);

    QChar randChr(QStringView charset);
    QString randStr(int length                      = 6);
    QString randStr(QStringView charset, int length = 6);

    QString relPath(const QString &dirpath, QString path);
    QString relNamespace(const QString &dirpath, QString path);



    QString toNamespacedID(const QString &dirpath, QStringView filepath,
                           bool noTagForm = false);

    QVariant strToVariant(QStringView str);
    QString variantToStr(const QVariant &vari);
    QVector<QString> fileIdList(const QString &dirpath,
                                const QString &catDir = QString(),
                                const QString &nspace = QString(),
                                bool noTagForm        = true,
                                bool from_1_21        = false);

    constexpr QStringView minecraftPrefix{ u"minecraft:" };

    bool removePrefix(QString &str, QLatin1String prefix);
    bool removePrefix(QString &str, QStringView prefix = minecraftPrefix);
    bool removeInternalPrefix(QString &path);

    QString canonicalCategory(QStringView catDir, const bool from_1_21);
    QStringView canonicalCategoryView(QStringView catDir, const bool from_1_21);


    extern const QMap<QString, QString> colorHexes;
    extern const QMap<char, QString>    colorCodes;
}

#endif /* GLOBALHELPERS_H */
