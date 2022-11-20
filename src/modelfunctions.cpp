#include "modelfunctions.h"

#include "mainwindow.h"
#include "extendedtablewidget.h"

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

void initComboModelView(const QString &infoType,
                        QStandardItemModel &model,
                        QComboBox *combo, bool optional, bool append) {
    if (optional)
        model.appendRow(new QStandardItem(QCoreApplication::translate(
                                              "BaseCondition",
                                              "(not set)")));
    const auto &&info = MainWindow::readMCRInfo(infoType);
    for (QString &key : info.keys()) {
        QStandardItem *item = new QStandardItem();
        if (info.value(key).toMap().contains(QStringLiteral("name")))
            item->setText(info.value(key).toMap()[QStringLiteral(
                                                      "name")].toString());
        else if (info.value(key).canConvert(QVariant::String)
                 && !info.value(key).isNull())
            item->setText(info.value(key).toString());
        else
            item->setText(key);
        QString &&iconPath =
            QString(":minecraft/texture/%1/%2.png").arg(infoType, key);
        QIcon icon(iconPath);
        if (!icon.pixmap(1, 1).isNull())
            item->setIcon(icon);
        if (!key.contains(':') && append)
            key.prepend(QLatin1String("minecraft:"));
        item->setData(key);
        item->setData(key, ExtendedRole::ComboboxDataRole);
        model.appendRow(item);
    }
    combo->setModel(&model);
}

void setupComboFrom(QComboBox *combo, const QVariant &vari, int role) {
    const auto *model = qobject_cast<QStandardItemModel*>(combo->model());

    if (vari.canConvert<QString>()) {
        QString &&str = vari.toString();
        if (!str.startsWith(QStringLiteral("minecraft:")))
            str.prepend(QStringLiteral("minecraft:"));
        for (int i = 0; i < model->rowCount(); ++i) {
            QString &&val = model->item(i, 0)->data(role).toString();
            if (val == str) {
                combo->setCurrentIndex(i);
                return;
            }
        }
    } else {
        for (int i = 0; i < model->rowCount(); ++i) {
            if (model->item(i, 0)->data(role) == vari) {
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
