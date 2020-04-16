#include "dialogdatabutton.h"
#include "ui_dialogdatabutton.h"

#include "basecondition.h"

#include <QDebug>

DialogDataButton::DialogDataButton(QWidget *parent) :
    QLabel(parent),
    ui(new Ui::DialogDataButton) {
    ui->setupUi(this);
    setMargin(30);

    connect(ui->button, &QPushButton::clicked,
            this, &DialogDataButton::onClicked);
    connect(ui->secondButton, &QToolButton::clicked,
            this, &DialogDataButton::onSecondaryClicked);
    checkSecondary();
}

DialogDataButton::~DialogDataButton() {
    delete ui;
}

void DialogDataButton::setText(const QString &str) {
    ui->button->setText(str);
    ui->button->adjustSize();
    adjustSize();
    QLabel::setText(str);
}

void DialogDataButton::onClicked() {
    switch (dialogType) {
    case ItemCond: {
        ItemConditionDialog dialog(this);
        dialog.fromJson(data);
        if (dialog.exec())
            setData(dialog.toJson());
        break;
    }

    case LocationCond: {
        LocationConditionDialog dialog(this);
        dialog.fromJson(data);
        if (dialog.exec())
            setData(dialog.toJson());
        break;
    }

    case EntityCond:
        EntityConditionDialog dialog(this);
        dialog.fromJson(data);
        if (dialog.exec())
            setData(dialog.toJson());
        break;
    }
}

void DialogDataButton::onSecondaryClicked() {
    setData(QJsonObject());
}

void DialogDataButton::checkSecondary() {
    ui->secondButton->setDisabled(data.isEmpty());
}

QJsonObject DialogDataButton::getData() const {
    qDebug() << "getData" << objectName() << data;
    return data;
}

void DialogDataButton::setData(const QJsonObject &value) {
    qDebug() << "setData" << objectName() << value;
    data = value;
    checkSecondary();
}

void DialogDataButton::setDialogType(const Type &value) {
    dialogType = value;
}
