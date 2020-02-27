#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog) {
    ui->setupUi(this);

    setupLanguageSetting();

    QSettings settings;
    ui->reloadExternChangesCombo->setCurrentIndex
        (settings.value("general/reloadExternChanges", 0).toInt());

    connect(this, &QDialog::accepted, this, &SettingsDialog::onAccepted);
}

void SettingsDialog::setupLanguageSetting() {
    QString m_langPath = QApplication::applicationDirPath()
                         + "/translations";
    QDir dir(m_langPath);
    //qDebug() << dir.exists() << dir.isEmpty();
    QStringList fileNames = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

    //qDebug() << fileNames.count() << dir.count();

    ui->languageCombo->addItem(tr("<Default>"), "");
    for (int i = 0; i < fileNames.size(); ++i) {
        QString locale = fileNames[i].section('.', 0, 0).section('_', 1);

        QString lang = QString("%1 (%2)").arg(
            QLocale::languageToString(QLocale(locale).language()),
            QLocale(locale).nativeLanguageName());
        //QIcon ico(QString("%1_%2.png").arg(m_langPath).arg(locale));

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
    settings.endGroup();
    qobject_cast<MainWindow*>(parent())->readPrefSettings(settings);
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}
