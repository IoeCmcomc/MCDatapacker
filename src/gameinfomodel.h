#ifndef GAMEINFOMODEL_H
#define GAMEINFOMODEL_H

#include <QAbstractListModel>

QT_BEGIN_NAMESPACE
class QFileSystemWatcher;
class QCompleter;
QT_END_NAMESPACE

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
        Default         = HasOptionalItem | PrependPrefix,
        IdsAreTags      = 4,
        ForceKeyAsName  = 8,
        DontShowIcons   = 16,
    };
    Q_DECLARE_FLAGS(Options, Option)
    Q_FLAG(Options)

    enum ItemRole {
        IdRole = Qt::UserRole + 1,
    };

    explicit GameInfoModel(QObject *parent = nullptr);
    explicit GameInfoModel(QObject *parent, const QString &key,
                           LoadFrom loadFrom, Options options = Default,
                           const QString &packCategory        = {});

    void setSource(const QString &key,
                   LoadFrom loadFrom, Options options           = Default);
    void setInfo(const QString &key, Options options            = Default);
    void setRegistry(const QString &key, Options options        = Default);
    void setDatapackCategory(const QString &cat, bool autoWatch = false);
    void setOptionalItem(bool value);
    QCompleter * createCompleter();

    int rowCount(const QModelIndex &parent = QModelIndex()) const final;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const final;


public slots:
    void updateDatapackIds();

private:
    QFileSystemWatcher *m_watcher = nullptr;
    QString m_key;
    QString m_dtpCategory;
    QVariantMap m_data;
    QVector<QString> m_dataIds;
    QVector<QString> m_datapackIds;
    LoadFrom m_loadFrom = Info;
    Options m_options   = Default;

    int staticRowCount() const;
    void loadData();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GameInfoModel::Options)

#endif // GAMEINFOMODEL_H
