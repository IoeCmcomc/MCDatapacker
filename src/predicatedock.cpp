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
    ui->conditionsContainer->setLayout(&conditionsLayout);

    onAdded(); /* Add an initial condition widget */
    Q_ASSERT(!ui->conditionsContainer->children().isEmpty());
    if (MainWindow::getCurGameVersion() >= QVersionNumber(1, 16)) {
    } else {
        qobject_cast<MCRPredCondition*>(ui->conditionsContainer->children()[0])
        ->setIsModular(false);
        ui->addBtn->hide();
    }
    resize(minimumWidth(), height());

    connect(ui->readBtn, &QPushButton::clicked,
            this, &PredicateDock::onReadBtn);
    connect(ui->writeBtn, &QPushButton::clicked,
            this, &PredicateDock::onWriteBtn);
    connect(ui->addBtn, &QToolButton::clicked,
            this, &PredicateDock::onAdded);
    connect(this, &QDockWidget::topLevelChanged, [ = ](bool floating) {
        adjustSize();
        qDebug() << minimumWidth() << width() << floating;
        if (floating) {
            resize(minimumWidth(), height());
        }
    });

    auto *cond = new MCRPredCondition();
    cond->setMinimumHeight(1000);
    ui->dataInterface->setMainWidget(cond);

    ui->dataInterface->mapToSetter(
        cond, qOverload<const QJsonObject &>(&MCRPredCondition::fromJson));

    ui->dataInterface->mapToGetter(&MCRPredCondition::toJson, cond);

/*
      connect(ui->dataInterface,
              &DataWidgetInterface::setterCallRequested,
              cond,
              qOverload<const QJsonObject &>(&MCRPredCondition::fromJson));
 */

/*
      connect(ui->dataInterface,
              &DataWidgetInterface::setterCallRequested, cond,
              [ = ](const QJsonObject &obj) {
          cond->fromJson(obj);
      });
 */
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
    QString input =
        qobject_cast<MainWindow*>(parent())->getCodeEditorText();
    QJsonDocument json_doc = QJsonDocument::fromJson(input.toUtf8());

    if (json_doc.isNull() || (!json_doc.isObject()))
        return;

    if (MainWindow::getCurGameVersion() >= QVersionNumber(1, 16)
        && json_doc.isArray()) {
        auto conditions = json_doc.array();
        Glhp::loadJsonToObjectsToLayout<MCRPredCondition>(conditions,
                                                          conditionsLayout);
    } else {
        QJsonObject root = json_doc.object();
        if (root.isEmpty())
            return;

        Glhp::deleteChildrenIn(ui->conditionsContainer);
        onAdded();
        qDebug() << ui->conditionsContainer->children().size();

        auto *first = qobject_cast<MCRPredCondition*>(
            ui->conditionsContainer->children()[0]);
        Q_ASSERT(first);
        first->fromJson(root);
    }
}

void PredicateDock::onWriteBtn() {
    QJsonDocument jsonDoc;
    const auto   &children = ui->conditionsContainer->children();

    if (MainWindow::getCurGameVersion() >= QVersionNumber(1, 16)
        && children.size() != 1) {
        const auto &&conditions =
            Glhp::getJsonFromObjectsFromParent<MCRPredCondition>(
                ui->conditionsContainer);
        jsonDoc = QJsonDocument(conditions);
    } else {
        const auto *first = qobject_cast<MCRPredCondition*>(children[0]);
        Q_ASSERT(first);
        jsonDoc = QJsonDocument(first->toJson());
    }

    qobject_cast<MainWindow*>(parent())->setCodeEditorText(jsonDoc.toJson());
}

void PredicateDock::onAdded() {
    qDebug() << "PredicateDock::onAdded";
    auto *cond = new MCRPredCondition(ui->conditionsContainer);

    conditionsLayout.addWidget(cond, 0);
}
