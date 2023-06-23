#include "itemmodifierdock.h"
#include "ui_itemmodifierdock.h"

#include "mainwindow.h"
#include "loottablefunction.h"

#include "game.h"
#include "platforms/windows_specific.h"

#include <QJsonDocument>
#include <QJsonObject>


ItemModifierDock::ItemModifierDock(QWidget *parent) :
    QDockWidget(parent), ui(new Ui::ItemModifierDock) {
    ui->setupUi(this);

    connect(ui->readBtn, &QPushButton::clicked,
            this, &ItemModifierDock::onReadBtn);
    connect(ui->writeBtn, &QPushButton::clicked,
            this, &ItemModifierDock::onWriteBtn);
    connect(this, &QDockWidget::topLevelChanged, [ = ](bool floating) {
        adjustSize();
        if (floating) {
            Windows::setDarkFrameIfDarkMode(this);
            resize(minimumWidth(), height());
        }
    });

    auto *func = new LootTableFunction();
    ui->dataInterface->setMainWidget(func);

    ui->dataInterface->mapToSetter(
        func, qOverload<const QJsonObject &>(&LootTableFunction::fromJson));

    ui->dataInterface->mapToGetter(&LootTableFunction::toJson, func);

    ui->dataInterface->addAfterCurrent();
}

ItemModifierDock::~ItemModifierDock() {
    delete ui;
}

void ItemModifierDock::changeEvent(QEvent *event) {
    QDockWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}

void ItemModifierDock::onReadBtn() {
    QString &&input =
        qobject_cast<MainWindow *>(parent())->getCodeEditorText();
    QJsonDocument &&json_doc = QJsonDocument::fromJson(input.toUtf8());

    if (json_doc.isNull() || json_doc.isEmpty())
        return;

    if (Game::version() >= Game::v1_16
        && json_doc.isArray()) {
        ui->dataInterface->setJson(json_doc.array());
    } else {
        QJsonObject root = json_doc.object();
        if (root.isEmpty())
            return;

        ui->dataInterface->setJson({ root });
    }
}

void ItemModifierDock::onWriteBtn() {
    QJsonDocument jsonDoc;

    if (Game::version() >= Game::v1_16
        && ui->dataInterface->entriesCount() != 1) {
        jsonDoc = QJsonDocument(ui->dataInterface->json());
    } else {
        jsonDoc = QJsonDocument(ui->dataInterface->json()[0].toObject());
    }

    qobject_cast<MainWindow *>(parent())->setCodeEditorText(jsonDoc.toJson());
}
