#include "extendeddelegate.h"

#include <QDebug>
#include <QJsonValue>
#include <QJsonObject>
#include <QPainter>
#include <QApplication>

ExtendedDelegate::ExtendedDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {
}

bool isExtendedNumeric(const QModelIndex &index) {
    return index.data().isValid()
           && index.data().canConvert<QJsonValue>()
           && (index.data().toJsonValue().isDouble()
               || index.data().toJsonValue().isObject());
}

void ExtendedDelegate::paint(QPainter *painter,
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
            } else {
                QString min = (obj.contains(QStringLiteral("min")))
                              ? QString::number(
                    obj.value(QStringLiteral("min")).toInt())
                              : QStringLiteral("");
                QString max = (obj.contains(QStringLiteral("max")))
                                  ? QString::number(
                    obj.value(QStringLiteral("max")).toInt())
                                  : QStringLiteral("");
                qDebug() << "delegate::paint" << min << max;
                newOption.text = QString("%1..%2").arg(min).arg(max);
            }
        }

        qApp->style()->drawControl(QStyle::CE_ItemViewItem,
                                   &newOption, painter, nullptr);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QWidget *ExtendedDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const {
    if (isExtendedNumeric(index)) {
        auto *editor = new NumericInput(parent);
        editor->setAutoFillBackground(true);
        editor->setTypes(ExNumInputTypes);
        editor->setGeneralMinimum(NumInputGeneralMin);
        editor->setGeneralMaximum(NumInputGeneralMax);

        connect(editor, &NumericInput::editingFinished,
                this, &ExtendedDelegate::commitAndCloseEditor);
        return editor;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void ExtendedDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const {
    if (isExtendedNumeric(index)) {
        QJsonValue jsonVal   = index.data().toJsonValue();
        auto      *numEditor = qobject_cast<NumericInput*>(editor);
        numEditor->fromJson(jsonVal);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void ExtendedDelegate::setModelData(QWidget *editor,
                                    QAbstractItemModel *model,
                                    const QModelIndex &index) const {
    if (isExtendedNumeric(index)) {
        auto *numEditor = qobject_cast<NumericInput*>(editor);
        numEditor->interpretText();
        model->setData(index, numEditor->toJson(), Qt::EditRole);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void ExtendedDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index)
const {
    editor->setGeometry(option.rect);
}

void ExtendedDelegate::commitAndCloseEditor() {
    auto *editor = qobject_cast<NumericInput*>(sender());
    emit  commitData(editor);
    emit  closeEditor(editor);
}

void ExtendedDelegate::setExNumInputGeneralMax(int value) {
    NumInputGeneralMax = value;
}

void ExtendedDelegate::setExNumInputGeneralMin(int value) {
    NumInputGeneralMin = value;
}

NumericInput::Types ExtendedDelegate::getExNumInputTypes() const {
    return ExNumInputTypes;
}

void ExtendedDelegate::setExNumInputTypes(
    const NumericInput::Types &value) {
    ExNumInputTypes = value;
}
