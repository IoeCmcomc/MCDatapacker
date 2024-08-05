#ifndef DIALOGDATABUTTON_H
#define DIALOGDATABUTTON_H

#include <QJsonObject>
#include <QPushButton>

#include <type_traits>

QT_BEGIN_NAMESPACE
class QDialog;
QT_END_NAMESPACE

namespace Ui {
    class DialogDataButton;
}

class DialogDataButton : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QJsonObject data READ getData WRITE setData NOTIFY dataChanged)

public:
    explicit DialogDataButton(QWidget *parent = nullptr);
    ~DialogDataButton();

    void setText(const QString &str);
    QString text() const;

    QJsonObject getData() const;
    void setData(const QJsonObject &value, const bool emitChanged = true);

    template<class T, typename =
                 std::enable_if_t<std::is_base_of_v<QDialog, T>, void> >
    void assignDialogClass() {
        if (!m_dialogClassAssigned) {
            connect(button(), &QPushButton::clicked, this, [this]() {
                T dialog(this);
                dialog.fromJson(m_data);
                if (dialog.exec())
                    setData(dialog.toJson());
            }, Qt::UniqueConnection);
            m_dialogClassAssigned = true;
        } else {
            qWarning() << "The dialog class has been assigned.";
        }
    }

signals:
    void dataChanged(const QJsonValue &value);

public slots:
    void reset(const bool emitChanged = true);

private:
    QJsonObject m_data;
    Ui::DialogDataButton *ui;
    bool m_dialogClassAssigned = false;

    QPushButton *button();
    void checkSecondary();
};

#endif /* DIALOGDATABUTTON_H */
