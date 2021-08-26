#include "dialogdatabutton.h"
#include "ui_dialogdatabutton.h"

#include "basecondition.h"
#include "itemconditiondialog.h"
#include "locationconditiondialog.h"
#include "entityconditiondialog.h"

#include <QDebug>

DialogDataButton::DialogDataButton(QWidget *parent) :
    QLabel(parent), ui(new Ui::DialogDataButton) {
    ui->setupUi(this);
    setMargin(30);

    connect(ui->secondButton, &QToolButton::clicked,
            this, &DialogDataButton::reset);
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

void DialogDataButton::reset() {
    setData(QJsonObject());
}

void DialogDataButton::checkSecondary() {
    ui->secondButton->setDisabled(data.isEmpty());
}

QJsonObject DialogDataButton::getData() const {
    return data;
}

void DialogDataButton::setData(const QJsonObject &value) {
    data = value;
    checkSecondary();
}
