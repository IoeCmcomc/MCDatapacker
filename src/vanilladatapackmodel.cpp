#include "vanilladatapackmodel.h"

#include "game.h"
#include "globalhelpers.h"

#include <QDirIterator>
#include <QAbstractItemModelTester>

VanillaNodeItem::VanillaNodeItem(const QString &name, VanillaNodeItem *parent)
    : m_name{name}, m_parent{parent} {
}

VanillaNodeItem::~VanillaNodeItem() {
    qDeleteAll(m_children);
}

bool VanillaNodeItem::isFile() const {
    return m_isFile;
}

QString VanillaNodeItem::name() const {
    return m_name;
}

int VanillaNodeItem::row() const {
    if (m_parent)
        return m_parent->m_children.indexOf(const_cast<VanillaNodeItem *>(this));

    return 0;
}

VanillaNodeItem * VanillaNodeItem::parent() const {
    return m_parent;
}

int VanillaNodeItem::childCount() const {
    return m_children.size();
}

void VanillaNodeItem::appendChild(VanillaNodeItem *child) {
    Q_ASSERT(child != nullptr);
    m_children.append(child);
    m_childLookups.insert(child->name(), child);
}

VanillaNodeItem * VanillaNodeItem::child(int row) {
    if (row < 0 || row >= m_children.size())
        return nullptr;

    return m_children.at(row);
}

VanillaNodeItem::VanillaNodeItem(bool isFile, VanillaNodeItem *parent)
    : m_parent{parent}, m_isFile{isFile} {
}

void VanillaNodeItem::setName(const QString &name) {
    m_name = name;
}

VanillaFileItem::VanillaFileItem(const QString &path, VanillaNodeItem *parent)
    : VanillaNodeItem(true, parent), m_file{path} {
    setName(m_file.info.fileName());
}

CodeFile VanillaFileItem::file() const {
    return m_file;
}


VanillaDatapackModel::VanillaDatapackModel(QObject *parent)
    : QAbstractItemModel{parent} {
    setupNodeItems();
    new QAbstractItemModelTester(this,
                                 QAbstractItemModelTester::FailureReportingMode::Warning,
                                 this);
}

Qt::ItemFlags VanillaDatapackModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    auto flags = QAbstractItemModel::flags(index);
    if (static_cast<VanillaNodeItem *>(index.internalPointer())->isFile()) {
        flags |= Qt::ItemNeverHasChildren;
    }
    return flags;
}

int VanillaDatapackModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0)
        return 0;

    VanillaNodeItem const *parentItem;
    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<VanillaNodeItem *>(parent.internalPointer());

    return parentItem->childCount();
}

int VanillaDatapackModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 1;
}

QVariant VanillaDatapackModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};

    const auto *item =
        static_cast<VanillaNodeItem *>(index.internalPointer());
    const VanillaFileItem *fileItem = nullptr;
    if (item->isFile()) {
        fileItem = static_cast<const VanillaFileItem *>(item);
    }

    switch (role) {
        case Qt::DisplayRole: {
            return item->name();
        }
        case Qt::DecorationRole: {
            if (fileItem) {
                return Glhp::fileTypeToIcon(fileItem->file().fileType);
            }
            return {};
        }
        case Qt::ToolTipRole: {
            if (fileItem) {
                return fileItem->file().path();
            }
            return {};
        }

        default:
            return {};
    }
    return {};
}

QModelIndex VanillaDatapackModel::index(int row, int column,
                                        const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    VanillaNodeItem *parentItem = nullptr;

    if (!parent.isValid()) {
        parentItem = m_rootItem;
    } else {
        parentItem = static_cast<VanillaNodeItem *>(parent.internalPointer());
    }

    if (VanillaNodeItem *childItem = parentItem->child(row)) {
        return createIndex(row, column, childItem);
    }

    return QModelIndex();
}

QModelIndex VanillaDatapackModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    VanillaNodeItem *item =
        static_cast<VanillaNodeItem *>(index.internalPointer());
    VanillaNodeItem *parentItem = item->parent();

    if (parentItem == m_rootItem)
        return QModelIndex();

    Q_ASSERT(parentItem != nullptr);

    return createIndex(parentItem->row(), 0, parentItem);
}

QVariant VanillaDatapackModel::headerData(
    int section, Qt::Orientation orientation, int role) const {
    return QVariant();
}

bool VanillaDatapackModel::isFile(const QModelIndex &index) const {
    if (!index.isValid())
        return false;

    return static_cast<VanillaNodeItem *>(index.internalPointer())->isFile();
}

QString VanillaDatapackModel::filePath(const QModelIndex &index) const {
    if (!index.isValid())
        return {};

    const auto *item =
        static_cast<VanillaNodeItem *>(index.internalPointer());
    if (item->isFile()) {
        return static_cast<const VanillaFileItem *>(item)->file().path();
    } else {
        return {};
    }
}

void VanillaDatapackModel::insertAliasToRoot(const QString &vanillaPath,
                                             const QString &category,
                                             const QString &alias) {
    const QString   &path  = category + '/' + alias;
    VanillaNodeItem *item  = m_rootItem;
    auto           &&parts = path.split('/');

    parts.removeLast();

    for (const QString &part: parts) {
        Q_ASSERT(item != nullptr);
        auto findIt = item->m_childLookups.constFind(part);

        if (findIt != item->m_childLookups.cend()) {
            item = findIt.value();
        } else {
            auto *childItem = new VanillaNodeItem(part, item);
            item->appendChild(childItem);
            item = childItem;
        }
    }
    const QString &&fullPath = vanillaPath + path;
    auto           *fileItem = new VanillaFileItem(fullPath, item);
    item->appendChild(fileItem);
}

void VanillaDatapackModel::setupNodeItems() {
    const QString &&vanillaPath = ":/minecraft/"_QL1 + Game::versionString() +
                                  "/data-json/data/minecraft/"_QL1;

    const VanillaLookupMap &lookups = Game::getVainllaLookupMap();

    m_rootItem = new VanillaNodeItem();
    Q_ASSERT(m_rootItem != nullptr);
    for (auto catIt = lookups.cbegin(); catIt != lookups.cend(); ++catIt) {
        const QString category = catIt.key();
        const auto   &aliases  = *catIt;
        for (auto aliasIt = aliases.cbegin(); aliasIt != aliases.cend();
             ++aliasIt) {
            const QString &alias = aliasIt.key();
            insertAliasToRoot(vanillaPath, category, alias);
        }
        QDir dir(vanillaPath + category);
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        QDirIterator it(dir, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QString &alias = dir.relativeFilePath(it.next());
            insertAliasToRoot(vanillaPath, category, alias);
        }
    }
}
