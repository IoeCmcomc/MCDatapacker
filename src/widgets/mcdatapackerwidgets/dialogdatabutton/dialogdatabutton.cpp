#include "dialogdatabutton.h"
#include "ui_dialogdatabutton.h"

#include <QDebug>

DialogDataButton::DialogDataButton(QWidget *parent) :
    QWidget(parent), ui(new Ui::DialogDataButton) {
    ui->setupUi(this);

    connect(ui->secondButton, &QToolButton::clicked,
            this, &DialogDataButton::reset);
    checkSecondary();
}

DialogDataButton::~DialogDataButton() {
    delete ui;
}

void DialogDataButton::setText(const QString &str) {
    ui->button->setText(str);
}

QString DialogDataButton::text() const {
    return ui->button->text();
}

void DialogDataButton::reset(const bool emitChanged) {
    setData(QJsonObject(), emitChanged);
}

QPushButton *DialogDataButton::button() {
    return ui->button;
}

void DialogDataButton::checkSecondary() {
    ui->secondButton->setDisabled(m_data.isEmpty());
}

QJsonObject DialogDataButton::getData() const {
    return m_data;
}

void DialogDataButton::setData(const QJsonObject &value, const bool emitChanged) {
    m_data = value;
    checkSecondary();
    if (emitChanged) {
        emit dataChanged(value);
    }
}
