#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    setupLanguageSetting();

    connect(this, &QDialog::accepted, this, &SettingsDialog::onAccepted);
}

void SettingsDialog::setupLanguageSetting() {

     QString m_langPath = QApplication::applicationDirPath()
             + "/translations";
     QDir dir(m_langPath);
     qDebug() << dir.exists() << dir.isEmpty();
     QStringList fileNames = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
     qDebug() << fileNames.count() << dir.count();

     ui->languageCombo->addItem(tr("<Default>"), "");
     for (int i = 0; i < fileNames.size(); ++i) {
          QString locale = fileNames[i];
          qDebug() << locale;
          locale = locale.section('.', 0, 0).section('_', 1);

         QString lang = QLocale::languageToString(QLocale(locale).language());
         //QIcon ico(QString("%1_%2.png").arg(m_langPath).arg(locale));

         qDebug() << lang << locale;
         ui->languageCombo->addItem(lang, locale);
     }

     ui->languageCombo->addItem(
                 QLocale::languageToString(QLocale("en_US").language()), "en_US");

     auto currLang = QLocale::languageToString(
                    QLocale(qobject_cast<MainWindow*>(parent())->getCurLocale())
             .language());
     if(currLang.isEmpty())
         ui->languageCombo->setCurrentText(0);
     else
        ui->languageCombo->setCurrentText(currLang);
}

void SettingsDialog::onAccepted() {
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.beginGroup("general");
    settings.setValue("locale", ui->languageCombo->currentData().toString());
    qDebug() << ui->languageCombo->currentData().toString();
    settings.endGroup();
    qobject_cast<MainWindow*>(parent())->readPrefSettings(settings);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
