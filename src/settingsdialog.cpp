#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "parsers/command/schema/schemarootnode.h"
#include "parsers/command/schema/schemaloader.h"

//#include "mainwindow.h"
#include "platforms/windows_specific.h"
#include "game.h"

#include <QOperatingSystemVersion>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QDir>
#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::SettingsDialog) {
    ui->setupUi(this);

#ifdef Q_OS_WIN
    Windows::applyGlassEffectToFrame(this, true);
    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        ui->tabWidget->widget(i)->setAutoFillBackground(true);
    }
#else
    ui->darkThemeLabel->hide();
    ui->darkThemeCombo->hide();
#endif
    setupLanguageSetting();
    initSettings();

    connect(this, &QDialog::accepted, this, &SettingsDialog::onAccepted);
    connect(ui->customCmdBtn, &QToolButton::clicked,
            this, &SettingsDialog::onCustomCmdBrowse);
    connect(ui->customCmdEdit, &QLineEdit::editingFinished,
            this, &SettingsDialog::checkCustomCmd);
}

void SettingsDialog::setupLanguageSetting() {
    ui->languageCombo->addItem(tr("(System locale)"), QString());

    QDir dir(QStringLiteral(":/i18n"));
    dir.setNameFilters({ QStringLiteral("MCDatapacker_*.qm") });

    QStringList &&fileNames = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

    dir.setPath(QApplication::applicationDirPath() +
                QStringLiteral("/translations"));

    fileNames += dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    fileNames.removeDuplicates();

    for (const QString &fileName: qAsConst(fileNames)) {
        QString &&localeCode = fileName.section('.', 0, 0).section('_', 1);
        QLocale   locale(localeCode);

        QString &&lang = QString("%1 (%2)").arg(
            QLocale::languageToString(locale.language()),
            locale.nativeLanguageName());

        ui->languageCombo->addItem(lang, localeCode);
    }

    const QString &localeCode =
        m_settings.value(QStringLiteral("interface/locale"),
                         QString()).toString();
    if (localeCode.isEmpty()) {
        ui->languageCombo->setCurrentText(0);
    } else {
        const QLocale &&curLocale = localeCode;
        auto          &&currLang  = QString("%1 (%2)").arg(
            curLocale.languageToString(curLocale.language()),
            curLocale.nativeLanguageName());
        ui->languageCombo->setCurrentText(currLang);
    }
}

void SettingsDialog::onAccepted() {
    m_settings.beginGroup("general");
    m_settings.setValue("reloadExternChanges",
                        ui->reloadExternChangesCombo->currentIndex());
    m_settings.endGroup();

    m_settings.beginGroup("interface");
    m_settings.setValue("locale", ui->languageCombo->currentData().toString());
    m_settings.setValue("style", ui->themeCombo->currentText());
    m_settings.setValue("darkStyle", ui->darkThemeCombo->currentText());
    m_settings.setValue("fontSizeScale",
                        ui->fontScaleCombo->currentText().chopped(1));
    m_settings.endGroup();

    m_settings.beginGroup("game");
    m_settings.setValue("version", ui->gameVersionCombo->currentText());
    m_settings.setValue("customCommandSyntaxFilePath",
                        ui->customCmdEdit->text());
    m_settings.endGroup();

    m_settings.beginGroup("editor");
    m_settings.setValue("textSize", ui->editorTextSizeSpin->value());
    m_settings.setValue("textFont", ui->editorTextFontCombo->currentFont());
    m_settings.setValue("wrap", ui->editorWrapCheck->isChecked());
    m_settings.setValue("toggleComments",
                        ui->commentToggleModeRadio->isChecked());
    m_settings.setValue("tabSize", ui->editorTabSizeSpin->value());
    m_settings.setValue("insertTabAsSpaces",
                        ui->editorTabAsSpacesCheck->isChecked());
    m_settings.setValue("showSpacesAndTabs",
                        ui->editorShowSpacesCheck->isChecked());
    m_settings.endGroup();

    m_settings.sync();
}

void SettingsDialog::onCustomCmdBrowse() {
    ui->customCmdEdit->setText(
        QFileDialog::getOpenFileName(
            this, tr("Open command syntax file"), "",
            tr("Command syntax tree files (*.json *.msgpack)")
            )
        );
    checkCustomCmd();
}

void SettingsDialog::checkCustomCmd() {
    const auto &&filepath = ui->customCmdEdit->text();

    if (filepath.isEmpty()) {
        ui->customCmdStatus->clear();
        return;
    }

    Command::Schema::SchemaLoader loader(ui->customCmdEdit->text());

    if (const QString && error = loader.lastError(); error.isEmpty()) {
        ui->customCmdStatus->setText(
            tr(
                R"(<b style="color:green">Valid</b>. %n command(s) detected.)",
                nullptr, loader.tree()->literalChildren().size()));
    } else {
        ui->customCmdStatus->setText(
            tr(R"(<b style="color:red">Error</b>: %1.)").arg(error));
    }

    delete loader.tree();
}

void SettingsDialog::initSettings() {
    m_settings.beginGroup(QStringLiteral("general"));
    ui->reloadExternChangesCombo->setCurrentIndex
        (m_settings.value(QStringLiteral("reloadExternChanges"), 0).toInt());
    m_settings.endGroup();

    m_settings.beginGroup(QStringLiteral("interface"));
    auto &&styles = QStyleFactory::keys();
    styles << QStringLiteral("DarkFusion") << QStringLiteral("NorwegianWood")
           << QStringLiteral("Qlementine");

    ui->themeCombo->addItems(styles);
    ui->darkThemeCombo->addItems(styles);

    ui->themeCombo->setCurrentText(
        m_settings.value(QStringLiteral("style"),
                         qApp->style()->objectName()).toString());
    ui->darkThemeCombo->setCurrentText(
        m_settings.value(QStringLiteral("darkStyle"),
                         QStringLiteral("DarkFusion")).toString());
    ui->fontScaleCombo->setCurrentText(
        m_settings.value(QStringLiteral("fontSizeScale"),
                         QStringLiteral("100")).toString() + QLatin1Char('%'));
    m_settings.endGroup();

    m_settings.beginGroup(QStringLiteral("game"));
    const auto &versionFinfos =
        QDir(QStringLiteral(":/minecraft")).entryInfoList({ "1.*" });
    for (const auto &finfo: versionFinfos)
        ui->gameVersionCombo->addItem(finfo.fileName());
    ui->gameVersionCombo->setCurrentText(
        m_settings.value(QStringLiteral("version"),
                         Game::defaultVersionString).toString());
    ui->customCmdEdit->setText(
        m_settings.value(QStringLiteral(
                             "customCommandSyntaxFilePath")).toString());
    checkCustomCmd();
    m_settings.endGroup();

    m_settings.beginGroup(QStringLiteral("editor"));
    ui->editorTextSizeSpin->setValue(m_settings.value(QStringLiteral("textSize"),
                                                      13).toInt());
    if (m_settings.contains(QStringLiteral("textFont"))) {
        ui->editorTextFontCombo->setCurrentFont(
            qvariant_cast<QFont>(m_settings.value(QStringLiteral("textFont"))));
    } else {
        ui->editorTextFontCombo->setCurrentFont(QFontDatabase::systemFont(
                                                    QFontDatabase::FixedFont));
    }
    ui->editorWrapCheck->setChecked(m_settings.value("wrap", false).toBool());
    if (m_settings.value(QStringLiteral("toggleComments"), false).toBool())
        ui->commentToggleModeRadio->setChecked(true);
    ui->editorTabSizeSpin->setValue(m_settings.value("tabSize", 4).toInt());
    if (!m_settings.value(QStringLiteral("insertTabAsSpaces"), true).toBool())
        ui->editorTabAsSpacesCheck->setChecked(false);
    if (m_settings.value(QStringLiteral("showSpacesAndTabs"), false).toBool())
        ui->editorShowSpacesCheck->setChecked(true);
    m_settings.endGroup();
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}
