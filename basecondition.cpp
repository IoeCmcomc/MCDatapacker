#include "basecondition.h"

#include "mainwindow.h"

#include <QApplication>

/*BaseCondition::BaseCondition(QWidget *parent) : QWidget(parent) { */
BaseCondition::BaseCondition() {
}

void BaseCondition::initModelView(QStandardItemModel &model,
                                  QTableView *tableView,
                                  std::initializer_list<QStandardItem *> headers,
                                  QAbstractItemDelegate *delegate) {
    int i = 0;

    for (auto header : headers) {
        model.setHorizontalHeaderItem(i, header);
        ++i;
    }

    tableView->setModel(&model);
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
                                              "(not selected)")));
    auto info = MainWindow::readMCRInfo(infoType);
    for (auto key : info.keys()) {
        QStandardItem *item = new QStandardItem();
        if (info.value(key).toMap().contains("name"))
            item->setText(info.value(key).toMap()["name"].toString());
        else if (info.value(key).canConvert(QVariant::String))
            item->setText(info.value(key).toString());
        else
            item->setText(key);
        auto iconPath =
            QString(":minecraft/texture/%1/%2.png").arg(infoType, key);
        auto icon = QIcon(iconPath);
        /*qDebug() << iconPath << icon; */
        if (!icon.pixmap(1, 1).isNull())
            item->setIcon(icon);
        item->setData("minecraft:" + key);
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
}
