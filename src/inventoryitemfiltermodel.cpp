#include "inventoryitemfiltermodel.h"

#include "inventoryitem.h"

#include <QTimer>

InventoryItemFilterModel::InventoryItemFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent) {
}

void InventoryItemFilterModel::setFilterRegularExpression(
    const QString &pattern) {
    const QRegularExpression regex(pattern);

    if (regex.isValid()) {
        QSortFilterProxyModel::setFilterRegularExpression(regex);
    } else {
        QSortFilterProxyModel::setFilterRegularExpression(QString());
    }
}

bool InventoryItemFilterModel::filterAcceptsRow(int sourceRow,
                                                const QModelIndex &sourceParent)
const {
    const QModelIndex &index =
        sourceModel()->index(sourceRow, 0, sourceParent);
    const auto &invItem = sourceModel()->data(index, Qt::UserRole +
                                              1).value<InventoryItem>();

    if (invItem.isBlock()) {
        if (invItem.isItem()) {
            if (!(m_filters & BlockItems)) {
                return false;
            }
        } else if (!(m_filters & UnobtainableBlocks)) {
            return false;
        }
    } else if (invItem.isItem()) {
        if (!(m_filters & NonblockItem)) {
            return false;
        }
    }

    return invItem.getNamespacedID().contains(filterRegularExpression()) ||
           invItem.getName().toCaseFolded().contains(filterRegularExpression());
}

InventoryItemFilterModel::Filters InventoryItemFilterModel::filters() const {
    return m_filters;
}

InventoryItemFilterModel::Filters &InventoryItemFilterModel::filtersRef() {
    QTimer::singleShot(0, this, [this]() {
        invalidateFilter();
    });
    return m_filters;
}

void InventoryItemFilterModel::setFilters(const Filters &filter) {
    m_filters = filter;
    invalidateFilter();
}
