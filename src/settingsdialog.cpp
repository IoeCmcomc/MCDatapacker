#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "mainwindow.h"
#include "platforms/windows.h"

#include <QOperatingSystemVersion>
#include <QFontDatabase>
#include <QSettings>
#include <QStyleFactory>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog) {
    ui->setupUi(this);

    qDebug() << qApp->style()->objectName();

    extendFrameOnWindows(this, "SettingsDialog");

    setupLanguageSetting();

    initSettings();

    connect(this, &QDialog::accepted, this, &SettingsDialog::onAccepted);
}

void SettingsDialog::setupLanguageSetting() {
    ui->languageCombo->addItem(tr("<Default>"), QString());

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

    auto &&curLocale =
        QLocale(qobject_cast<MainWindow*>(parent())->getCurLocale());
    auto &&currLang = QString("%1 (%2)").arg(
        curLocale.languageToString(curLocale.language()),
        curLocale.nativeLanguageName());
    if (currLang.isEmpty())
        ui->languageCombo->setCurrentText(0);
    else
        ui->languageCombo->setCurrentText(currLang);
}

void SettingsDialog::onAccepted() {
    QSettings settings(QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());

    settings.setValue("theme", ui->themeCombo->currentText());
    settings.beginGroup("general");
    settings.setValue("locale", ui->languageCombo->currentData().toString());
    settings.setValue("reloadExternChanges",
                      ui->reloadExternChangesCombo->currentIndex());
    settings.setValue("gameVersion",
                      ui->gameVersionCombo->currentText());
    settings.endGroup();
    settings.beginGroup("editor");
    settings.setValue("textSize", ui->editorTextSizeSpin->value());
    settings.setValue("textFont", ui->editorTextFontCombo->currentFont());
    settings.setValue("wrap", ui->editorWrapCheck->isChecked());
    settings.setValue("toggleComments",
                      ui->commentToggleModeRadio->isChecked());
    settings.setValue("tabSize", ui->editorTabSizeSpin->value());
    settings.setValue("insertTabAsSpaces",
                      ui->editorTabAsSpacesCheck->isChecked());
    settings.setValue("showSpacesAndTabs",
                      ui->editorShowSpacesCheck->isChecked());
    settings.endGroup();

    qobject_cast<MainWindow*>(parent())->readPrefSettings(settings, true);
}

void SettingsDialog::initSettings() {
    QSettings settings;

    const auto &&styles = QStyleFactory::keys();
    for (const auto &style: styles) {
        ui->themeCombo->addItem(style);
    }
    ui->themeCombo->setCurrentText(settings.value(QStringLiteral("theme"),
                                                  qApp->style()->objectName()).toString());

    settings.beginGroup(QStringLiteral("general"));
    ui->reloadExternChangesCombo->setCurrentIndex
        (settings.value(QStringLiteral("reloadExternChanges"), 0).toInt());

    for (const auto &finfo:
         QDir(QStringLiteral(":/minecraft")).entryInfoList({ "1.*" }))
        ui->gameVersionCombo->addItem(finfo.fileName());
    ui->gameVersionCombo->setCurrentText(settings.value(QStringLiteral(
                                                            "gameVersion"),
                                                        "1.17").toString());
    settings.endGroup();

    settings.beginGroup(QStringLiteral("editor"));
    ui->editorTextSizeSpin->setValue(settings.value(QStringLiteral("textSize"),
                                                    13).toInt());
    if (settings.contains(QStringLiteral("textFont"))) {
        ui->editorTextFontCombo->setCurrentFont(qvariant_cast<QFont>(settings.
                                                                     value(
                                                                         QStringLiteral(
                                                                             "textFont"))));
    } else {
        ui->editorTextFontCombo->setCurrentFont(QFontDatabase::systemFont(
                                                    QFontDatabase::FixedFont));
    }
    ui->editorWrapCheck->setChecked(settings.value("wrap", false).toBool());
    if (settings.value(QStringLiteral("toggleComments"), false).toBool())
        ui->commentToggleModeRadio->setChecked(true);
    ui->editorTabSizeSpin->setValue(settings.value("tabSize", 4).toInt());
    if (!settings.value(QStringLiteral("insertTabAsSpaces"), true).toBool())
        ui->editorTabAsSpacesCheck->setChecked(false);
    if (settings.value(QStringLiteral("showSpacesAndTabs"), false).toBool())
        ui->editorShowSpacesCheck->setChecked(true);
    settings.endGroup();
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}
