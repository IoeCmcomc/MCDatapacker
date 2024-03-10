#include "gameinfomodel.h"

#include "game.h"

#include <QDebug>
#include <QIcon>
#include <QCoreApplication>

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

int GameInfoModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return (m_options & Option::HasOptionalItem)+m_data.size();
}

QVariant GameInfoModel::data(const QModelIndex &index, int role) const {
    const static QString iconPathString =
        QStringLiteral(":minecraft/texture/%1/%2.png");

    if (!index.isValid())
        return QVariant();

    const int row = index.row();
    QString   key;
    if (m_options & Option::HasOptionalItem) {
        if (row == 0) {
            if (role == Qt::DisplayRole) {
                return QCoreApplication::translate("BaseCondition",
                                                   "(not set)");
            } else {
                return {};
            }
        } else {
            key = m_dataKeys.at(row - 1);
        }
    } else {
        key = m_dataKeys.at(row);
    }
    const auto &item = m_data.value(key);

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
            QString &&iconPath = iconPathString.arg(m_key, key);
            QIcon     icon(iconPath);
            if (!icon.pixmap(1, 1).isNull()) {
                return icon;
            } else {
                return {};
            }
        }
        default:
            return {};
    }
}

void GameInfoModel::loadData() {
    if (m_loadFrom == GameInfoModel::Info) {
        m_data     = Game::getInfo(m_key);
        m_dataKeys = m_data.keys().toVector();
    } else {
        auto &&items = Game::getRegistry(m_key);
        m_dataKeys = items;
        for (const QString &item: qAsConst(items)) {
            m_data.insert(item, {});
        }
    }
}
