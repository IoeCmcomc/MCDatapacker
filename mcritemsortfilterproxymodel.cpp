#include "mcritemsortfilterproxymodel.h"

#include "mcrinvitem.h"

MCRItemSortFilterProxyModel::MCRItemSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent) {
}

bool MCRItemSortFilterProxyModel::filterAcceptsRow(int sourceRow,
                                                   const QModelIndex &sourceParent)
const {
    QModelIndex index   = sourceModel()->index(sourceRow, 0, sourceParent);
    MCRInvItem  invItem = sourceModel()->data(index,
                                              Qt::UserRole +
                                              1).value<MCRInvItem>();;

    if (invItem.getHasBlockForm()) {
        if (getFilterByBlock()) {
            /* cont */
        } else {
            return false;
        }
    } else {
        if (getFilterByItem()) {
            /* cont */
        } else {
            return false;
        }
    }

    return invItem.getNamespacedID().contains(filterRegularExpression()) ||
           invItem.getName().toLower().contains(filterRegularExpression());
}

bool MCRItemSortFilterProxyModel::getFilterByItem() const {
    return filterByItem;
}

void MCRItemSortFilterProxyModel::setFilterByItem(bool value) {
    filterByItem = value;
    invalidateFilter();
}

bool MCRItemSortFilterProxyModel::getFilterByBlock() const {
    return filterByBlock;
}

void MCRItemSortFilterProxyModel::setFilterByBlock(bool value) {
    filterByBlock = value;
    invalidateFilter();
}
