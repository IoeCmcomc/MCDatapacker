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

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(
        QVariant data READ data WRITE setData NOTIFY dataChanged RESET reset USER true)

public:
    explicit DialogDataButton(QWidget *parent = nullptr);
    ~DialogDataButton();

    void setText(const QString &str);
    QString text() const;

    QVariant data() const;
    void setData(const QVariant& value, const bool emitChanged = true);

    QJsonObject getJsonObj() const;
    void setJson(const QJsonObject &value, const bool emitChanged = true);

    QPushButton * button();

    template<class T, typename =
                 std::enable_if_t<std::is_base_of_v<QDialog, T>, void> >
    void assignDialogClass() {
        if (!m_dialogClassAssigned) {
            connect(button(), &QPushButton::clicked, this, [this]() {
                T dialog(this);
                dialog.fromJson(m_data.toJsonObject());
                if (dialog.exec())
                    setJson(dialog.toJson());
            }, Qt::UniqueConnection);
            m_dialogClassAssigned = true;
        } else {
            qWarning() << "The dialog class has been assigned.";
        }
    }

signals:
    void textChanged(const QString &text);
    void dataChanged(const QVariant &value);

public slots:
    void reset(const bool emitChanged = true);

private:
    QVariant m_data;
    Ui::DialogDataButton *ui;
    bool m_dialogClassAssigned = false;

    void checkSecondary();
};

#endif /* DIALOGDATABUTTON_H */
