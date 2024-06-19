#ifndef VANILLADATAPACKMODEL_H
#define VANILLADATAPACKMODEL_H

#include "codefile.h"

#include <QAbstractItemModel>

/*!
 * \class VanillaNodeItem
 * \brief Represent an item in a VanillaDatapackModel.
 */
class VanillaNodeItem {
public:
    explicit VanillaNodeItem(const QString &name     = QString(),
                             VanillaNodeItem *parent = nullptr);
    ~VanillaNodeItem();

    bool isFile() const;
    QString name() const;

    int row() const;
    VanillaNodeItem * parent() const;

    int childCount() const;
    void appendChild(VanillaNodeItem *child);
    VanillaNodeItem * child(int row);
    void sortChildren();

    friend class VanillaDatapackModel;

protected:
    VanillaNodeItem(bool isFile, VanillaNodeItem *parent = nullptr);
    void setName(const QString &name);

private:
    QVector<VanillaNodeItem *> m_children;
    QHash<QString, VanillaNodeItem *> m_childLookups;
    QString m_name;
    VanillaNodeItem *m_parent = nullptr;
    bool m_isFile             = false;
};

/*!
 * \class VanillaFileItem
 * \brief Represent an leaf node (a file) in a VanillaDatapackModel.
 */
class VanillaFileItem : public VanillaNodeItem {
public:
    explicit VanillaFileItem(const QString &path,
                             VanillaNodeItem *parent = nullptr);

    CodeFile file() const;

private:
    CodeFile m_file;
};

/*!
 * \class VanillaDatapackModel
 * \brief A data model provide read-only access to the folder structure of
 * the default datapack of the current game version.
 */
class VanillaDatapackModel : public QAbstractItemModel
{
public:
    explicit VanillaDatapackModel(QObject *parent = nullptr);

    Qt::ItemFlags flags(const QModelIndex &index) const final;
    int rowCount(const QModelIndex &parent    = QModelIndex()) const final;
    int columnCount(const QModelIndex &parent = QModelIndex()) const final;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const final;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const final;
    QModelIndex parent(const QModelIndex &index) const final;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const final;

    bool isFile(const QModelIndex &index) const;
    QString filePath(const QModelIndex &index) const;

private:
    VanillaNodeItem *m_rootItem = nullptr;

    void setupNodeItems();
    void insertAliasToRoot(const QString &vanillaPath,
                           const QString &category, const QString &alias);
};

#endif // VANILLADATAPACKMODEL_H
