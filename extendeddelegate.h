#ifndef EXTENDEDNUMERICDELEGATE_H
#define EXTENDEDNUMERICDELEGATE_H

#include "numericinput.h"

#include <QStyledItemDelegate>

class ExtendedDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ExtendedDelegate(QObject *parent = nullptr);

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

    NumericInput::Types getExNumInputTypes() const;
    void setExNumInputTypes(const NumericInput::Types &value);

    void setExNumInputGeneralMin(int value);
    void setExNumInputGeneralMax(int value);

private slots:
    void commitAndCloseEditor();

private:
    NumericInput::Types ExNumInputTypes;
    int NumInputGeneralMin = 0;
    int NumInputGeneralMax = INT_MAX;
};

#endif /* EXTENDEDNUMERICDELEGATE_H */
