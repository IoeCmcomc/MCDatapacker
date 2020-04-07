#ifndef EXTENDEDNUMERICDELEGATE_H
#define EXTENDEDNUMERICDELEGATE_H

#include "extendednumericinput.h"

#include <QStyledItemDelegate>

class ExtendedNumericDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ExtendedNumericDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

    ExtendedNumericInput::Types getExNumInputTypes() const;
    void setExNumInputTypes(const ExtendedNumericInput::Types &value);

    void setExNumInputGeneralMin(int value);
    void setExNumInputGeneralMax(int value);

private slots:
    void commitAndCloseEditor();

private:
    ExtendedNumericInput::Types ExNumInputTypes;
    int ExNumInputGeneralMin = 0;
    int ExNumInputGeneralMax = 999999999;
};

#endif /* EXTENDEDNUMERICDELEGATE_H */
