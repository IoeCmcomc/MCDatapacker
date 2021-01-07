#ifndef TAGSELECTORDIALOG_H
#define TAGSELECTORDIALOG_H

#include "mainwindow.h"
#include "codefile.h"

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
    explicit TagSelectorDialog(QWidget *parent         = nullptr,
                               CodeFile::FileType type = CodeFile::ItemTag);
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

    void setupTagTreeView(CodeFile::FileType type);
    QString getInternalSelectedID();
};

#endif /* TAGSELECTORDIALOG_H */
