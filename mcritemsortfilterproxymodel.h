#ifndef MCRITEMSORTFILTERPROXYMODEL_H
#define MCRITEMSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class MCRItemSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    MCRItemSortFilterProxyModel(QObject *parent = nullptr);

    bool getFilterByBlock() const;
    void setFilterByBlock(bool value);

    bool getFilterByItem() const;
    void setFilterByItem(bool value);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    bool filterByBlock = true;
    bool filterByItem  = true;
};

#endif // MCRITEMSORTFILTERPROXYMODEL_H
