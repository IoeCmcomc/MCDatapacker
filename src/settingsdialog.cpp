#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "mainwindow.h"

#include <QtWin>
#include <QOperatingSystemVersion>
#include <QFontDatabase>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog) {
    ui->setupUi(this);

    if (QOperatingSystemVersion::current() <
        QOperatingSystemVersion::Windows8) {
        if (QtWin::isCompositionEnabled()) {
            QtWin::extendFrameIntoClientArea(this, -1, -1, -1, -1);
            setAttribute(Qt::WA_TranslucentBackground, true);
            setAttribute(Qt::WA_NoSystemBackground, false);
            setStyleSheet(QStringLiteral(
                              "SettingsDialog, QTabWidget { background: transparent; }"));
        } else {
            QtWin::resetExtendedFrame(this);
            setAttribute(Qt::WA_TranslucentBackground, false);
        }
    }

    setupLanguageSetting();

    initSettings();

    connect(this, &QDialog::accepted, this, &SettingsDialog::onAccepted);
}

void SettingsDialog::setupLanguageSetting() {
    ui->languageCombo->addItem(tr("<Default>"), "");

    QDir dir(QStringLiteral(":/i18n"));
    dir.setNameFilters({ QStringLiteral("*.qm") });

    QStringList fileNames = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

    dir.setPath(QApplication::applicationDirPath() +
                QStringLiteral("/translations"));

    fileNames += dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    fileNames.removeDuplicates();

    for (int i = 0; i < fileNames.size(); ++i) {
        QString locale = fileNames[i].section('.', 0, 0).section('_', 1);

        QString lang = QString("%1 (%2)").arg(
            QLocale::languageToString(QLocale(locale).language()),
            QLocale(locale).nativeLanguageName());
        /*QIcon ico(QString("%1_%2.png").arg(m_langPath).arg(locale)); */

        ui->languageCombo->addItem(lang, locale);
    }


    ui->languageCombo->addItem(QString("%1 (%2)").arg(
                                   QLocale::languageToString(QLocale("en_US").
                                                             language()),
                                   QLocale("en_US").nativeLanguageName()),
                               "en_US");

    auto curLocale =
        QLocale(qobject_cast<MainWindow*>(parent())->getCurLocale());
    auto currLang = QString("%1 (%2)").arg(
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
    settings.endGroup();

    qobject_cast<MainWindow*>(parent())->readPrefSettings(settings);
}

void SettingsDialog::initSettings() {
    QSettings settings;

    settings.beginGroup("general");
    ui->reloadExternChangesCombo->setCurrentIndex
        (settings.value("reloadExternChanges", 0).toInt());

    for (const auto &finfo: QDir(":/minecraft").entryInfoList({ "1.*" }))
        ui->gameVersionCombo->addItem(finfo.fileName());
    ui->gameVersionCombo->setCurrentText(settings.value("gameVersion",
                                                        0).toString());
    settings.endGroup();

    settings.beginGroup("editor");
    ui->editorTextSizeSpin->setValue(settings.value("textSize", 13).toInt());
    if (settings.contains("textFont")) {
        ui->editorTextFontCombo->setCurrentFont(qvariant_cast<QFont>(settings.
                                                                     value(
                                                                         "textFont")));
    } else {
        ui->editorTextFontCombo->setCurrentFont(QFontDatabase::systemFont(
                                                    QFontDatabase::FixedFont));
    }
    ui->editorWrapCheck->setChecked(settings.value("wrap", false).toBool());
    if (settings.value("toggleComments", false).toBool())
        ui->commentToggleModeRadio->setChecked(true);
    settings.endGroup();
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}
