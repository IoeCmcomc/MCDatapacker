#ifndef DIALOGDATABUTTON_H
#define DIALOGDATABUTTON_H

#include "ui_dialogdatabutton.h"

#include <QJsonObject>
#include <QLabel>


QT_BEGIN_NAMESPACE
class QDialog;
QT_END_NAMESPACE

namespace Ui {
    class DialogDataButton;
}

class DialogDataButton : public QWidget
{
    Q_OBJECT

public:
    explicit DialogDataButton(QWidget *parent = nullptr);
    ~DialogDataButton();

    void setText(const QString &str);

    QJsonObject getData() const;
    void setData(const QJsonObject &value);

    template<class T, typename =
                 std::enable_if_t<std::is_base_of_v<QDialog, T>, void> >
    void assignDialogClass() {
        if (!m_dialogClassAssigned) {
            connect(ui->button, &QPushButton::clicked, this, [this]() {
                T dialog(this);
                dialog.fromJson(data);
                if (dialog.exec())
                    setData(dialog.toJson());
            }, Qt::UniqueConnection);
            m_dialogClassAssigned = true;
        } else {
            qWarning() << "The dialog class has been assigned.";
        }
    }

public slots:
    void reset();

private:
    QJsonObject data;
    Ui::DialogDataButton *ui;
    bool m_dialogClassAssigned = false;

    void checkSecondary();
};

#endif /* DIALOGDATABUTTON_H */
