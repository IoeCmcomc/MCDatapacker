#ifndef TAGSELECTORDIALOG_H
#define TAGSELECTORDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QPushButton>

namespace Ui {
    class TagSelectorDialog;
}

class TagSelectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagSelectorDialog(QWidget *parent = nullptr);
    ~TagSelectorDialog();

    QString getSelectedID();

protected slots:
    void checkOK();
    void showDetails();

private:
    Ui::TagSelectorDialog *ui;
    QStandardItemModel model;
    QSortFilterProxyModel filterModel;
    QPushButton *selectButton;
    QMap<QString, QVariant> MCRTagInfo;

    void setupTagTreeView();
    QString getInternalSelectedID();
};

#endif /* TAGSELECTORDIALOG_H */
