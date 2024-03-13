#include "gameinfomodel.h"

#include "game.h"
#include "globalhelpers.h"

#include <QDebug>
#include <QIcon>
#include <QCoreApplication>
#include <QDir>
#include <QFileSystemWatcher>

GameInfoModel::GameInfoModel(QObject *parent)
    : QAbstractListModel(parent) {
}

GameInfoModel::GameInfoModel(QObject *parent, const QString &key,
                             LoadFrom loadFrom, Options options)
    : QAbstractListModel(parent), m_key{key}, m_loadFrom{loadFrom},
    m_options{options} {
    loadData();
}

void GameInfoModel::setSource(const QString &key, LoadFrom loadFrom,
                              Options options) {
    m_key      = key;
    m_loadFrom = loadFrom;
    m_options  = options;
    beginResetModel();
    loadData();
    endResetModel();
}

void GameInfoModel::setDatapackCategory(const QString &cat, bool autoWatch) {
    m_dtpCategory = cat;

    if (autoWatch) {
        if (m_watcher) {
            const auto &oldDirs = m_watcher->directories();

            if (!oldDirs.isEmpty()) {
                m_watcher->removePaths(oldDirs);
            }
        } else {
            m_watcher = new QFileSystemWatcher(this);
        }
        Q_ASSERT(m_watcher != nullptr);

        const QString &dataPath = QDir::currentPath() +
                                  QStringLiteral("/data/");
        QDir         dir(dataPath);
        const auto &&nspaceDirs = dir.entryList(
            QDir::Dirs | QDir::NoDotAndDotDot);

        for (const auto &nspaceDir : nspaceDirs) {
            const QString &&watchPath = dataPath +
                                        nspaceDir.section('.', 0,
                                                          0) + "/" + cat;
            if (QFile::exists(watchPath)) {
                qDebug() << watchPath;
                m_watcher->addPath(watchPath);
            }
        }

        connect(m_watcher, &QFileSystemWatcher::directoryChanged,
                this, &GameInfoModel::updateDatapackIds);
    } else if (m_watcher) {
        m_watcher->deleteLater();
        m_watcher = nullptr;
    }

    updateDatapackIds();
}

void GameInfoModel::updateDatapackIds() {
    const auto      &ids = Glhp::fileIdList(QDir::currentPath(), m_dtpCategory);
    QVector<QString> filteredIds;

    if (!m_data.isEmpty()) {
        for (const QString &newId: ids) {
            QString noMinecraftPrefixId = newId;
            Glhp::removePrefix(noMinecraftPrefixId);
            if (!m_data.contains(noMinecraftPrefixId)) {
                filteredIds << std::move(newId);
            }
        }
    } else {
        filteredIds = ids;
    }
    if (rowCount() - 1 >= staticRowCount()) {
        beginResetModel();
        m_datapackIds = filteredIds;
        endResetModel();
    } else {
        beginInsertRows({}, GameInfoModel::rowCount(),
                        GameInfoModel::rowCount() + filteredIds.size());
        m_datapackIds = filteredIds;
        endInsertRows();
    }
}

int GameInfoModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return staticRowCount() + m_datapackIds.size();
}

QVariant GameInfoModel::data(const QModelIndex &index, int role) const {
    const static QString iconPathString =
        QStringLiteral(":minecraft/texture/%1/%2.png");

    if (!index.isValid())
        return QVariant();

    const int row         = index.row();
    const int datapackRow = row - staticRowCount();
    QString   key;
    QVariant  item;
    if (m_options & Option::HasOptionalItem) {
        if (row == 0) {
            if (role == Qt::DisplayRole) {
                return QCoreApplication::translate("BaseCondition",
                                                   "(not set)");
            } else {
                return {};
            }
        } else if (datapackRow >= 0) {
            key = m_datapackIds.at(datapackRow);
        } else {
            key  = m_dataIds.at(row - 1);
            item = m_data.value(key);
        }
    } else if (datapackRow >= 0) {
        key = m_datapackIds.at(datapackRow);
    } else {
        key  = m_dataIds.at(row);
        item = m_data.value(key);
    }

    switch (role) {
        case Qt::DisplayRole: {
            if (item.isNull()) {
                return key;
            } else if (!(m_options & Option::ForceKeyAsName)) {
                const auto &map = item.toMap();
                if (map.contains(QStringLiteral("name"))) {
                    return map[QStringLiteral("name")].toString();
                }else if (item.canConvert(QVariant::String)) {
                    return item.toString();
                } else {
                    return key;
                }
            }
            [[fallthrough]];
        }
        case Qt::ToolTipRole:
        case Qt::EditRole:
        case ItemRole::IdRole: {
            if (m_options & Option::PrependPrefix && !key.contains(':')) {
                if (m_options & Option::IdsAreTags) {
                    key.prepend(QLatin1String("#minecraft:"));
                } else {
                    key.prepend(QLatin1String("minecraft:"));
                }
            }
            return key;
        }
        case Qt::DecorationRole: {
            if (datapackRow < 0 && !(m_options & Option::DontShowIcons)) {
                QString &&iconPath = iconPathString.arg(m_key, key);
                QIcon     icon(iconPath);
                if (!icon.pixmap(1, 1).isNull()) {
                    return icon;
                } else {
                    return {};
                }
            } else {
                return {};
            }
        }
        default:
            return {};
    }
}

int GameInfoModel::staticRowCount() const {
    return (m_options & Option::HasOptionalItem)+m_data.size();
}

void GameInfoModel::loadData() {
    if (m_key.isEmpty()) {
        m_data.clear();
        m_dataIds.clear();
    } else if (m_loadFrom == GameInfoModel::Info) {
        m_data    = Game::getInfo(m_key);
        m_dataIds = m_data.keys().toVector();
    } else {
        auto &&items = Game::getRegistry(m_key);
        m_dataIds = items;
        for (const QString &item: qAsConst(items)) {
            m_data.insert(item, {});
        }
    }
}
