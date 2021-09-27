#include "comboboxdelegate.h"

#include "extendedtablewidget.h"

#include <QDebug>
#include <QJsonValue>
#include <QJsonObject>
#include <QComboBox>
#include <QTimer>

ComboboxDelegate::ComboboxDelegate(QComboBox *editor, QObject *parent)
    : QStyledItemDelegate(parent) {
    Q_ASSERT(editor);
    m_editor = editor;
}

QWidget *ComboboxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const {
    if (!index.data(ExtendedRole::ComboboxIndexRole).isNull()) {
        auto *combobox = new QComboBox(parent);
        qDebug() << combobox;
        combobox->setFrame(false);
        combobox->setModel(m_editor->model());
        connect(combobox, qOverload<int>(&QComboBox::activated),
                this, &ComboboxDelegate::emitCommitData);
        return combobox;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void ComboboxDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const {
    if (!index.data(ExtendedRole::ComboboxIndexRole).isNull()) {
        if (m_isPopulated)
            return;
        else
            m_isPopulated = true;

        const int comboIndex =
            index.data(ExtendedRole::ComboboxIndexRole).toInt();
        qDebug() << editor << comboIndex;

        auto *combobox = qobject_cast<QComboBox*>(editor);
        combobox->setCurrentIndex(comboIndex);
        QTimer::singleShot(1, this, [combobox]() {
            combobox->showPopup();
        });
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void ComboboxDelegate::setModelData(QWidget *editor,
                                    QAbstractItemModel *model,
                                    const QModelIndex &index) const {
    if (!index.data(ExtendedRole::ComboboxIndexRole).isNull()) {
        auto *combobox = qobject_cast<QComboBox*>(editor);
        model->setData(index, combobox->currentData(Qt::DisplayRole),
                       Qt::DisplayRole);
        model->setData(index, combobox->currentData(Qt::DecorationRole),
                       Qt::DecorationRole);
        model->setData(index, combobox->currentIndex(),
                       ExtendedRole::ComboboxIndexRole);
        qDebug() << combobox << combobox->currentIndex() << index.data(
            ExtendedRole::ComboboxIndexRole);
        m_isPopulated = false;
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void ComboboxDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            [[maybe_unused]] const QModelIndex &index)
const {
    editor->setGeometry(option.rect);
}

void ComboboxDelegate::emitCommitData() {
    qDebug() << "commit";
    auto *w = qobject_cast<QComboBox*>(sender());
    emit  commitData(w);
    emit  closeEditor(w);
}
