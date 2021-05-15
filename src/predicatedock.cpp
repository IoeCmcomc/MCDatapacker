#include "predicatedock.h"
#include "ui_predicatedock.h"

#include "mainwindow.h"
#include "mcrpredcondition.h"

#include "globalhelpers.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

PredicateDock::PredicateDock(QWidget *parent) :
    QDockWidget(parent), ui(new Ui::PredicateDock) {
    qDebug() << minimumWidth() << width();
    ui->setupUi(this);

    if (MainWindow::getCurGameVersion() >= QVersionNumber(1, 16)) {
    } else {
    }
    resize(minimumWidth(), height());

    connect(ui->readBtn, &QPushButton::clicked,
            this, &PredicateDock::onReadBtn);
    connect(ui->writeBtn, &QPushButton::clicked,
            this, &PredicateDock::onWriteBtn);
    connect(this, &QDockWidget::topLevelChanged, [ = ](bool floating) {
        adjustSize();
        qDebug() << minimumWidth() << width() << floating;
        if (floating) {
            resize(minimumWidth(), height());
        }
    });

    auto *cond = new MCRPredCondition();
    /*cond->setMinimumHeight(500); */
    cond->setIsModular(true);
    ui->dataInterface->setMainWidget(cond);

    ui->dataInterface->mapToSetter(
        cond, qOverload<const QJsonObject &>(&MCRPredCondition::fromJson));

    ui->dataInterface->mapToGetter(&MCRPredCondition::toJson, cond);
}

PredicateDock::~PredicateDock() {
    delete ui;
}

void PredicateDock::changeEvent(QEvent *event) {
    QDockWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}

void PredicateDock::onReadBtn() {
    qDebug() << "PredicateDock::onReadBtn";
    QString &&input =
        qobject_cast<MainWindow*>(parent())->getCodeEditorText();
    QJsonDocument &&json_doc = QJsonDocument::fromJson(input.toUtf8());

    if (json_doc.isNull() || json_doc.isEmpty())
        return;

    if (MainWindow::getCurGameVersion() >= QVersionNumber(1, 16)
        && json_doc.isArray()) {
        ui->dataInterface->setJson(json_doc.array());
    } else {
        QJsonObject root = json_doc.object();
        if (root.isEmpty())
            return;

        ui->dataInterface->setJson({ root });
    }
}

void PredicateDock::onWriteBtn() {
    QJsonDocument jsonDoc;

    if (MainWindow::getCurGameVersion() >= QVersionNumber(1, 16)
        && ui->dataInterface->entriesCount() != 1) {
        jsonDoc = QJsonDocument(ui->dataInterface->json());
    } else {
        jsonDoc = QJsonDocument(ui->dataInterface->json()[0].toObject());
    }

    qobject_cast<MainWindow*>(parent())->setCodeEditorText(jsonDoc.toJson());
}
