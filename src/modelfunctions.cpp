#include "modelfunctions.h"

#include "game.h"
#include "extendedtablewidget.h"

#include <QListView>
#include <QApplication>

void initModelView(QStandardItemModel &model,
                   QTableView *tableView,
                   std::initializer_list<QStandardItem *> headers,
                   ViewEventFilter &viewFilter,
                   QAbstractItemDelegate *delegate) {
    tableView->setModel(&model);
    int i = 0;
    for (auto header : headers) {
        model.setHorizontalHeaderItem(i, header);
        ++i;
    }
    tableView->installEventFilter(&viewFilter);
    if (delegate != nullptr)
        tableView->setItemDelegate(delegate);
}

void initComboModelView(const QString &infoType, QStandardItemModel &model,
                        QComboBox *combo, bool optional, bool append,
                        bool asTag, bool forceKeyName) {
    if (optional)
        model.appendRow(new QStandardItem(QCoreApplication::translate(
                                              "BaseCondition",
                                              "(not set)")));
    const auto &&info = Game::getInfo(infoType);
    for (auto it = info.cbegin(); it != info.cend(); ++it) {
        QString        key  = it.key();
        QStandardItem *item = new QStandardItem();
        if (it.value().toMap().contains(QStringLiteral("name")))
            item->setText(it.value().toMap()[QStringLiteral(
                                                 "name")].toString());
        else if (it.value().canConvert(QVariant::String)
                 && !it.value().isNull() && !forceKeyName)
            item->setText(it.value().toString());
        else if (asTag)
            item->setText("#" + key);
        else
            item->setText(key);
        QString &&iconPath =
            QString(":minecraft/texture/%1/%2.png").arg(infoType, key);
        QIcon icon(iconPath);
        if (!icon.pixmap(1, 1).isNull())
            item->setIcon(icon);
        if (!key.contains(':') && append) {
            if (asTag) {
                key.prepend(QLatin1String("#minecraft:"));
            } else {
                key.prepend(QLatin1String("minecraft:"));
            }
        }

        item->setData(key);
        model.appendRow(item);
    }
    combo->setModel(&model);
}

void initComboModelViewFromRegistry(const QString &registry,
                                    QStandardItemModel &model,
                                    QComboBox *combo, bool optional,
                                    bool append) {
    if (optional)
        model.appendRow(new QStandardItem(QCoreApplication::translate(
                                              "BaseCondition",
                                              "(not set)")));
    const auto &&values = Game::getRegistry(registry);
    for (QString value : values) {
        QStandardItem *item     = new QStandardItem(value);
        QString      &&iconPath =
            QString(":minecraft/texture/%1/%2.png").arg(registry, value);
        QIcon icon(iconPath);
        if (!icon.pixmap(1, 1).isNull())
            item->setIcon(icon);
        if (!value.contains(':') && append) {
            value.prepend(QLatin1String("minecraft:"));
        }
        item->setData(value);
        model.appendRow(item);
    }
    combo->setModel(&model);
}

void setComboValueFrom(QComboBox *combo, const QVariant &vari, int role) {
    const auto *model = qobject_cast<QAbstractItemModel *>(combo->model());

    Q_ASSERT(model != nullptr);

    if (vari.canConvert<QString>()) {
        QString &&str = vari.toString();
        if (!str.contains(':')) {
            if (str[0] == '#') {
                if (!str.startsWith(QStringLiteral("#minecraft:")))
                    str.prepend(QStringLiteral("#minecraft:"));
            } else {
                if (!str.startsWith(QStringLiteral("minecraft:")))
                    str.prepend(QStringLiteral("minecraft:"));
            }
        }

        for (int i = 0; i < model->rowCount(); ++i) {
            QString &&val = model->data(model->index(i, 0), role).toString();
            if (val == str) {
                combo->setCurrentIndex(i);
                return;
            }
        }
    } else {
        for (int i = 0; i < model->rowCount(); ++i) {
            if (model->data(model->index(i, 0), role) == vari) {
                combo->setCurrentIndex(i);
                return;
            }
        }
    }
    combo->setCurrentIndex(0);
}

void appendRowToTableWidget(QTableWidget *table,
                            std::initializer_list<QTableWidgetItem *> items) {
    table->insertRow(table->rowCount());
    int       i   = 0;
    const int row = table->rowCount() - 1;
    for (const auto &item : items) {
        table->setItem(row, i, item);
        ++i;
    }
}

void appendRowToModel(QStandardItemModel &model, const QString &text,
                      const QVariant &data, int role) {
    QStandardItem *item = new QStandardItem(text);

    item->setData(data, role);
    model.appendRow(item);
}

void hideComboRow(QComboBox *comboBox, const int row) {
    if (auto *view = qobject_cast<QListView *>(comboBox->view())) {
        auto *model = static_cast<QStandardItemModel *>(comboBox->model());
        view->setRowHidden(row, true);
        model->item(row, 0)->setEnabled(false);
    }
}
