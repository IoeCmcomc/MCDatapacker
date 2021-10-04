#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include "extendedtablewidget.h"

#include <QStandardItem>
#include <QStyledItemDelegate>

QT_BEGIN_NAMESPACE
class QComboBox;
QT_END_NAMESPACE

class ComboboxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboboxDelegate(QComboBox *editor, QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

private slots:
    void emitCommitData();

private:
    QComboBox *m_editor        = nullptr;
    mutable bool m_isPopulated = false;
};

#endif /* COMBOBOXDELEGATE_H */
