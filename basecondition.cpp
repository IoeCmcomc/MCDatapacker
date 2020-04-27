#include "basecondition.h"

#include "mainwindow.h"

#include <QApplication>

BaseCondition::BaseCondition() {
}

QJsonObject BaseCondition::toJson() const {
    return QJsonObject();
}

void BaseCondition::fromJson(const QJsonObject &value) {
}

void BaseCondition::initModelView(QStandardItemModel &model,
                                  QTableView *tableView,
                                  std::initializer_list<QStandardItem *> headers,
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

void BaseCondition::initComboModelView(const QString &infoType,
                                       QStandardItemModel &model,
                                       QComboBox *combo, bool optinal) {
    if (optinal)
        model.appendRow(new QStandardItem(QCoreApplication::translate(
                                              "BaseCondition",
                                              "(not set)")));
    auto info = MainWindow::readMCRInfo(infoType);
    for (auto key : info.keys()) {
        QStandardItem *item = new QStandardItem();
        if (info.value(key).toMap().contains(QStringLiteral("name")))
            item->setText(info.value(key).toMap()[QStringLiteral(
                                                      "name")].toString());
        else if (info.value(key).canConvert(QVariant::String)
                 && !info.value(key).isNull())
            item->setText(info.value(key).toString());
        else
            item->setText(key);
        auto iconPath =
            QString(":minecraft/texture/%1/%2.png").arg(infoType, key);
        auto icon = QIcon(iconPath);
        if (!icon.pixmap(1, 1).isNull())
            item->setIcon(icon);
        if (!key.contains(QStringLiteral(":")))
            key = QStringLiteral("minecraft:") + key;
        item->setData(key);
        model.appendRow(item);
    }
    combo->setModel(&model);
}

void BaseCondition::setupComboFrom(QComboBox *combo, const QVariant &vari,
                                   int role) {
    auto *model = qobject_cast<QStandardItemModel*>(combo->model());

    for (int i = 0; i < model->rowCount(); ++i) {
        if (model->item(i, 0)->data(role) == vari) {
            combo->setCurrentIndex(i);
            return;
        }
    }
    combo->setCurrentIndex(0);
}

void BaseCondition::appendRowToTableWidget(QTableWidget *table,
                                           std::initializer_list<QTableWidgetItem *> items)
{
    table->insertRow(table->rowCount());
    int       i   = 0;
    const int row = table->rowCount() - 1;
    for (auto item : items) {
        table->setItem(row, i, item);
        ++i;
    }
}
