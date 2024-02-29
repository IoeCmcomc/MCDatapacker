#include "loottableeditordock.h"
#include "ui_loottableeditordock.h"

#include "mainwindow.h"
#include "platforms/windows_specific.h"

#include <QDebug>
#include <QJsonDocument>


LootTableEditorDock::LootTableEditorDock(QWidget *parent) :
    QDockWidget(parent), ui(new Ui::LootTableEditorDock) {
    ui->setupUi(this);
    m_mainWin = qobject_cast<MainWindow *>(this->parent());
    Q_ASSERT(m_mainWin != nullptr);

    connect(ui->writeLootTableBtn, &QPushButton::clicked,
            this, &LootTableEditorDock::writeJson);
    connect(ui->readLootTableBtn, &QPushButton::clicked,
            this, &LootTableEditorDock::readJson);

    connect(this, &QDockWidget::topLevelChanged, [ = ](bool floating) {
        adjustSize();
        if (floating) {
            Windows::setDarkFrameIfDarkMode(this);
            resize(minimumWidth(), height());
        }
    });
}

LootTableEditorDock::~LootTableEditorDock() {
    delete ui;
}

void LootTableEditorDock::writeJson() {
    m_mainWin->setCodeEditorText(QJsonDocument(ui->lootTable->toJson()).toJson());
}

void LootTableEditorDock::readJson() {
    QString &&input = m_mainWin->getCodeEditorText();

    ui->lootTable->fromJson(QJsonDocument::fromJson(input.toUtf8()).object());
}

void LootTableEditorDock::changeEvent(QEvent *event) {
    QDockWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}
