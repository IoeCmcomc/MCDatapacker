#include "predicatedock.h"
#include "ui_predicatedock.h"

#include "mainwindow.h"

#include <QJsonDocument>

PredicateDock::PredicateDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PredicateDock) {
    ui->setupUi(this);

    ui->condition->setIsModular(false);

    connect(ui->writeBtn, &QPushButton::clicked,
            this, &PredicateDock::onWriteBtn);
}

PredicateDock::~PredicateDock() {
    delete ui;
}

void PredicateDock::onReadBtn() {
}

void PredicateDock::onWriteBtn() {
    auto          condJson = ui->condition->toJson();
    QJsonDocument jsonDoc(condJson);

    qobject_cast<MainWindow*>(parent())->setCodeEditorText(jsonDoc.toJson());
}
