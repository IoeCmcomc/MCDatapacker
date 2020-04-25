#include "predicatedock.h"
#include "ui_predicatedock.h"

#include "mainwindow.h"

#include <QDebug>
#include <QJsonDocument>

PredicateDock::PredicateDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PredicateDock) {
    ui->setupUi(this);

    ui->condition->setIsModular(false);

    connect(ui->readBtn, &QPushButton::clicked,
            this, &PredicateDock::onReadBtn);
    connect(ui->writeBtn, &QPushButton::clicked,
            this, &PredicateDock::onWriteBtn);
}

PredicateDock::~PredicateDock() {
    delete ui;
}

void PredicateDock::onReadBtn() {
    qDebug() << "onReadBtn" << objectName();
    QString input =
        qobject_cast<MainWindow*>(parent())->getCodeEditorText();
    QJsonDocument json_doc = QJsonDocument::fromJson(input.toUtf8());

    if (json_doc.isNull() || (!json_doc.isObject()))
        return;

    QJsonObject root = json_doc.object();
    if (!root.isEmpty()) {
        ui->condition->fromJson(root);
    }
}

void PredicateDock::onWriteBtn() {
    auto          condJson = ui->condition->toJson();
    QJsonDocument jsonDoc(condJson);

    qobject_cast<MainWindow*>(parent())->setCodeEditorText(jsonDoc.toJson());
}
