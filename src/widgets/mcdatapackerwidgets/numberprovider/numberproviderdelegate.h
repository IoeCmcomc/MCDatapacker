#ifndef NUMBERPROVIDERDELEGATE_H
#define NUMBERPROVIDERDELEGATE_H

#include "numberprovider.h"
#include "extendedtablewidget.h"

#include <QStandardItem>
#include <QStyledItemDelegate>

class NumberProviderDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    NumberProviderDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option,
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

    NumberProvider::Modes inputModes() const;
    void setInputModes(const NumberProvider::Modes &value);

    void setMinLimit(int value);
    void setMaxLimit(int value);

    void setIntegerOnly(bool newIntegerOnly);

private slots:
    void commitAndCloseEditor();

private:
    NumberProvider::Modes m_inputModes;
    int m_minLimit = 0;
    int m_maxLimit = INT_MAX;
    bool m_integerOnly = false;

    QString textRepr(const QJsonValue &value) const;
};

#endif /* NUMBERPROVIDERDELEGATE_H */
