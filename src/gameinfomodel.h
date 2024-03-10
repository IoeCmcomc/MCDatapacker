#ifndef GAMEINFOMODEL_H
#define GAMEINFOMODEL_H

#include <QAbstractListModel>

/*!
 * \class GameInfoModel
 * \brief An item model whose underlying data is a list of a type of game
 * information or registry.
 *
 */
class GameInfoModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum LoadFrom {
        Info,
        Registry,
    };

    enum Option {
        NoOptions       = 0,
        HasOptionalItem = 1,
        PrependPrefix   = 2,
        IdsAreTags      = 4,
        ForceKeyAsName  = 8,
    };
    Q_DECLARE_FLAGS(Options, Option)
    Q_FLAG(Options)

    enum ItemRole {
        IdRole = Qt::UserRole + 1,
    };

    explicit GameInfoModel(QObject *parent = nullptr);
    explicit GameInfoModel(QObject *parent, const QString &key,
                           LoadFrom loadFrom, Options options = NoOptions);

    void setSource(const QString &key, LoadFrom loadFrom,
                   Options options = NoOptions);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;

private:
    QString m_key;
    QVariantMap m_data;
    QVector<QString> m_dataKeys;
    LoadFrom m_loadFrom = Info;
    Options m_options   = NoOptions;

    void loadData();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GameInfoModel::Options)

#endif // GAMEINFOMODEL_H
