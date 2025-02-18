#include "variantmapfile.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCborValue>
#include <QCborMap>

VariantMapFile::VariantMapFile(const QVariantMap &map) : variantMap{map} {
}

bool VariantMapFile::fromJsonFile(const QString &filePath,
                                  const bool checkExists) {
    QFile file(filePath);

    if (!checkExists || Q_LIKELY(file.exists())) {
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            const QByteArray &&data = file.readAll();
            file.close();

            QJsonParseError       errorPtr;
            const QJsonDocument &&doc =
                QJsonDocument::fromJson(data, &errorPtr);
            if (doc.isNull()) {
                m_errorMessage = errorPtr.errorString();
                return false;
            }
            QJsonObject &&root = doc.object();
            if (root.isEmpty()) {
                qWarning() << "The root of the JSON object is empty.";
            }

            variantMap = root.toVariantMap();
            return true;
        } else {
            m_errorMessage = file.errorString();
            return false;
        }
    } else {
        return false;
    }
}

bool VariantMapFile::fromCborFile(const QString &filePath,
                                  const bool checkExists) {
    QFile file(filePath);

    if (!checkExists || Q_LIKELY(file.exists())) {
        if (file.open(QIODevice::ReadOnly)) {
            const QByteArray &&data = file.readAll();
            file.close();

            QCborParserError   errorPtr;
            const QCborValue &&rootValue =
                QCborValue::fromCbor(data, &errorPtr);
            if (rootValue.isInvalid()) {
                m_errorMessage = errorPtr.errorString();
                return false;
            }
            QCborMap &&root = rootValue.toMap();
            if (root.isEmpty()) {
                qWarning() << "The root of the CBOR object is empty.";
            }

            variantMap = root.toVariantMap();
            return true;
        } else {
            m_errorMessage = file.errorString();
            return false;
        }
    } else {
        return false;
    }
}

bool VariantMapFile::fromFile(const QString &filePath) {
    if (QFile::exists(filePath)) {
        if (filePath.endsWith(".json")) {
            return fromJsonFile(filePath, false);
        } else if (filePath.endsWith(".cbor")) {
            return fromCborFile(filePath, false);
        } else {
            qWarning() << "File not supported:" << filePath;
        }
    } else {
        QString &&newPath = filePath + QLatin1String(".json");
        if (QFile::exists(newPath)) {
            return fromJsonFile(newPath, false);
        } else {
            newPath = filePath + QLatin1String(".cbor");
            if (QFile::exists(newPath)) {
                return fromCborFile(newPath, false);
            } else {
                qWarning() << "File not supported:" << filePath;
            }
        }
    }
    return false;
}

QString VariantMapFile::errorMessage() const
{
    return m_errorMessage;
}
