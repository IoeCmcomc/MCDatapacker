#ifndef TAGSELECTORDIALOG_H
#define TAGSELECTORDIALOG_H

#include "mainwindow.h"

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
    explicit TagSelectorDialog(QWidget *parent              = nullptr,
                               MainWindow::MCRFileType type = MainWindow::ItemTag);
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

    void setupTagTreeView(MainWindow::MCRFileType type);
    QString getInternalSelectedID();
};

#endif /* TAGSELECTORDIALOG_H */
