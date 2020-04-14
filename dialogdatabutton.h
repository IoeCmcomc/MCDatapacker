#ifndef DIALOGDATABUTTON_H
#define DIALOGDATABUTTON_H

#include "itemconditiondialog.h";
#include "locationconditiondialog.h"
#include "entityconditiondialog.h"

#include <QJsonObject>
#include <QLabel>

namespace Ui {
    class DialogDataButton;
}

class DialogDataButton : public QLabel
{
    Q_OBJECT

public:
    explicit DialogDataButton(QWidget *parent = nullptr);
    ~DialogDataButton();

    enum Type {
        ItemCond,
        LocationCond,
        EntityCond
    };

    void setText(const QString &str);

    void setDialogType(const Type &value);

    QJsonObject getData() const;
    void setData(const QJsonObject &value);

protected slots:
    void onClicked();
    void onSecondaryClicked();

private:
    Ui::DialogDataButton *ui;
    QJsonObject data;
    Type dialogType;

    void checkSecondary();
};

typedef DialogDataButton::Type DDBType;

#endif /* DIALOGDATABUTTON_H */
