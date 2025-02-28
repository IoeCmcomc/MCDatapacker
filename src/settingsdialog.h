#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

protected slots:
    void onAccepted();

private slots:
    void onCustomCmdBrowse();
    void checkCustomCmd();
    void onCustomCodePaletteBrowse();
    void onCustomCodePaletteGetSample();

private:
    Ui::SettingsDialog *ui;
    QSettings m_settings;

    void initSettings();
    void setupLanguageSetting();
};

#endif /* SETTINGSDIALOG_H */
