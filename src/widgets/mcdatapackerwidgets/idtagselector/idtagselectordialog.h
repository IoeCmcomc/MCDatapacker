#ifndef IDTAGSELECTORDIALOG_H
#define IDTAGSELECTORDIALOG_H

#include <QDialog>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
class QAbstractListModel;
QT_END_NAMESPACE

namespace Ui {
    class IdTagSelectorDialog;
}

class IdTagSelectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IdTagSelectorDialog(QWidget *parent = nullptr);
    ~IdTagSelectorDialog();

    void setIdDisplayName(const QString &name);
    void setIdModel(QAbstractListModel *model);

    void fromJson(const QJsonArray &arr);
    QJsonArray toJson() const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::IdTagSelectorDialog *ui;
};

#endif // IDTAGSELECTORDIALOG_H
