#include "extendednumericdelegate.h"

#include <QDebug>
#include <QJsonValue>
#include <QJsonObject>
#include <QPainter>
#include <QApplication>

ExtendedNumericDelegate::ExtendedNumericDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {
}

bool isExtendedNumeric(const QModelIndex &index) {
    return index.data().isValid()
           && index.data().canConvert<QJsonValue>()
           && (index.data().toJsonValue().isDouble()
               || index.data().toJsonValue().isObject());
}

void ExtendedNumericDelegate::paint(QPainter *painter,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const {
    if (index.isValid() && isExtendedNumeric(index)) {
        QJsonValue value = index.data().toJsonValue();

        auto newOption = option;
        if (value.isDouble()) {
            int ExactInt = value.toInt();
            newOption.text = QString::number(ExactInt);
        } else if (value.isObject()) {
            auto obj  = value.toObject();
            auto type = obj[QStringLiteral("type")].toString();
            if (type == QStringLiteral("minecraft:binomial")) {
                int    num  = obj.value(QStringLiteral("n")).toInt();
                double prob = obj.value(QStringLiteral("p")).toInt();
                newOption.text = QString("n: %1; p: %2").arg(num).arg(prob);
            } else if (obj.contains(QStringLiteral("min")) &&
                       obj.contains(QStringLiteral("max"))) {
                int min = obj.value(QStringLiteral("min")).toInt();
                int max = obj.value(QStringLiteral("max")).toInt();
                newOption.text = QString("%1..%2").arg(min).arg(max);
            }
        }

        qApp->style()->drawControl(QStyle::CE_ItemViewItem,
                                   &newOption, painter, nullptr);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QWidget *ExtendedNumericDelegate::createEditor(QWidget *parent,
                                               const QStyleOptionViewItem &option,
                                               const QModelIndex &index) const {
    if (isExtendedNumeric(index)) {
        auto *editor = new ExtendedNumericInput(parent);
        editor->setAutoFillBackground(true);
        editor->setTypes(ExNumInputTypes);
        connect(editor, &ExtendedNumericInput::editingFinished,
                this, &ExtendedNumericDelegate::commitAndCloseEditor);
        return editor;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void ExtendedNumericDelegate::setEditorData(QWidget *editor,
                                            const QModelIndex &index) const {
    if (isExtendedNumeric(index)) {
        QJsonValue jsonVal   = index.data().toJsonValue();
        auto      *numEditor = qobject_cast<ExtendedNumericInput*>(editor);
        numEditor->fromJson(jsonVal);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void ExtendedNumericDelegate::setModelData(QWidget *editor,
                                           QAbstractItemModel *model,
                                           const QModelIndex &index) const {
    if (isExtendedNumeric(index)) {
        auto *numEditor = qobject_cast<ExtendedNumericInput*>(editor);
        numEditor->interpretText();
        model->setData(index, numEditor->toJson(), Qt::EditRole);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void ExtendedNumericDelegate::updateEditorGeometry(QWidget *editor,
                                                   const QStyleOptionViewItem &option,
                                                   const QModelIndex &index)
const {
    editor->setGeometry(option.rect);
}

void ExtendedNumericDelegate::commitAndCloseEditor() {
    auto *editor = qobject_cast<ExtendedNumericInput*>(sender());
    emit  commitData(editor);
    emit  closeEditor(editor);
}

ExtendedNumericInput::Types ExtendedNumericDelegate::getExNumInputTypes() const
{
    return ExNumInputTypes;
}

void ExtendedNumericDelegate::setExNumInputTypes(
    const ExtendedNumericInput::Types &value) {
    ExNumInputTypes = value;
}
