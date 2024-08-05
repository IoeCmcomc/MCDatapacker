#ifndef NUMBERPROVIDERDIALOG_H
#define NUMBERPROVIDERDIALOG_H

#include "datawidgetcontroller.h"

#include <QDialog>

namespace Ui {
    class NumberProviderDialog;
}

class NumberProviderDialog : public QDialog {
    Q_OBJECT

public:
    explicit NumberProviderDialog(QWidget *parent = nullptr);
    ~NumberProviderDialog();

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &value);

protected:
    void changeEvent(QEvent *e);

private:
    DataWidgetControllerMultiPageWidget m_controller;
    Ui::NumberProviderDialog *ui;
    NumberProvider *m_parent = nullptr;
};

#endif // NUMBERPROVIDERDIALOG_H
