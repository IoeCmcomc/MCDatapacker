#ifndef INVENTORYITEMFILTERMODEL_H
#define INVENTORYITEMFILTERMODEL_H

#include <QSortFilterProxyModel>

class InventoryItemFilterModel : public QSortFilterProxyModel
{
public:
    enum Filter {
        UnobtainableBlocks = 1,
        ObtainableBlocks   = 2,
        AllBlocks          = ObtainableBlocks | UnobtainableBlocks,
        BlockItems         = ObtainableBlocks,
        NonblockItem       = 4,
        AllItems           = BlockItems | NonblockItem,
    };
    Q_DECLARE_FLAGS(Filters, Filter)

    InventoryItemFilterModel(QObject *parent = nullptr);

    void setFilterRegularExpression(const QString &pattern);

    Filters filters() const;
    Filters &filtersRef();
    void setFilters(const Filters &filter);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    Filters m_filters = { BlockItems, NonblockItem };
};

Q_DECLARE_OPERATORS_FOR_FLAGS(InventoryItemFilterModel::Filters)

#endif /* INVENTORYITEMFILTERMODEL_H */
