#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QDir>

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

private:
    Ui::SettingsDialog *ui;

    void initSettings();
    void setupLanguageSetting();
};

#endif /* SETTINGSDIALOG_H */
