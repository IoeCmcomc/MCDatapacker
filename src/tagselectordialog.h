#ifndef TAGSELECTORDIALOG_H
#define TAGSELECTORDIALOG_H

#include "codefile.h"

#include <QDialog>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QPushButton>

namespace Ui {
    class TagSelectorDialog;
}

class TagSelectorDialog : public QDialog {
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
    QSortFilterProxyModel filterModel;
    QStandardItemModel model;
    QVariantMap m_tagInfo;
    Ui::TagSelectorDialog *ui;
    QPushButton *selectButton = nullptr;

    void setupTagView(CodeFile::FileType type);
    QString getInternalSelectedID();
};

#endif /* TAGSELECTORDIALOG_H */
