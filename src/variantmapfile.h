#ifndef VARIANTMAPFILE_H
#define VARIANTMAPFILE_H

#include <QVariantMap>

class VariantMapFile
{
public:
    VariantMapFile(const QVariantMap &map);
    VariantMapFile() = default;

    bool fromJsonFile(const QString &filePath, const bool checkExists = true);

    bool fromCborFile(const QString &filePath, const bool checkExists = true);

    bool fromFile(const QString &filePath);

    QString errorMessage() const;

    QVariantMap variantMap;

private:
    QString m_errorMessage;
};

#endif // VARIANTMAPFILE_H
