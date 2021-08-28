#include "numberproviderdelegate.h"

#include <QDebug>
#include <QJsonValue>
#include <QJsonObject>
#include <QPainter>
#include <QApplication>
#include <QStylePainter>

NumberProviderDelegate::NumberProviderDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {
}

bool isExtendedNumeric(const QModelIndex &index) {
    return index.data().isValid()
           && index.data().canConvert<QJsonValue>()
           && (index.data().toJsonValue().isDouble()
               || index.data().toJsonValue().isObject());
}

void NumberProviderDelegate::paint(QPainter *painter,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const {
    if (index.isValid()) {
        if (isExtendedNumeric(index)) {
            QJsonValue value = index.data().toJsonValue();

            auto opt = option;
            initStyleOption(&opt, index);

            if (value.isDouble()) {
                int ExactInt = value.toInt();
                opt.text = QString::number(ExactInt);
            } else if (value.isObject()) {
                auto obj  = value.toObject();
                auto type = obj[QStringLiteral("type")].toString();
                if (type == QStringLiteral("minecraft:binomial")) {
                    int    num  = obj.value(QStringLiteral("n")).toInt();
                    double prob = obj.value(QStringLiteral("p")).toInt();
                    opt.text = QString("n: %1; p: %2").arg(num).arg(prob);
                } else {
                    QString min = (obj.contains(QStringLiteral("min")))
                              ? QString::number(
                        obj.value(QStringLiteral("min")).toInt())
                              : QLatin1String("");
                    QString max = (obj.contains(QStringLiteral("max")))
                                  ? QString::number(
                        obj.value(QStringLiteral("max")).toInt())
                                      : QString();
                    opt.text = QString("%1..%2").arg(min, max);
                }
            }

            const auto   *w     = option.widget;
            const QStyle *style = w ? w->style() : qApp->style();
            if (option.state & QStyle::State_Selected) {
                painter->setPen(Qt::white);
                painter->setBrush(option.palette.highlightedText());
            }else{
                painter->setPen(QPen(option.palette.windowText(), 0));
                painter->setBrush(qvariant_cast<QBrush>(
                                      index.data(Qt::ForegroundRole)));
            }
            style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, w);
        } else {
            QStyledItemDelegate::paint(painter, option, index);
        }
    }
}

QWidget *NumberProviderDelegate::createEditor(QWidget *parent,
                                              const QStyleOptionViewItem &option,
                                              const QModelIndex &index) const {
    if (isExtendedNumeric(index)) {
        auto *editor = new NumberProvider(parent);
        editor->setAutoFillBackground(true);
        editor->setModes(m_inputModes);
        editor->setMinLimit(m_minLimit);
        editor->setMaxLimit(m_maxLimit);

        connect(editor, &NumberProvider::editingFinished,
                this, &NumberProviderDelegate::commitAndCloseEditor);
        return editor;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void NumberProviderDelegate::setEditorData(QWidget *editor,
                                           const QModelIndex &index) const {
    if (isExtendedNumeric(index)) {
        QJsonValue jsonVal   = index.data().toJsonValue();
        auto      *numEditor = qobject_cast<NumberProvider*>(editor);
        numEditor->fromJson(jsonVal);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void NumberProviderDelegate::setModelData(QWidget *editor,
                                          QAbstractItemModel *model,
                                          const QModelIndex &index) const {
    if (isExtendedNumeric(index)) {
        auto *numEditor = qobject_cast<NumberProvider*>(editor);
        numEditor->interpretText();
        model->setData(index, numEditor->toJson(), Qt::EditRole);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void NumberProviderDelegate::updateEditorGeometry(QWidget *editor,
                                                  const QStyleOptionViewItem &option,
                                                  [[maybe_unused]] const QModelIndex &index)
const {
    editor->setGeometry(option.rect);
}

void NumberProviderDelegate::commitAndCloseEditor() {
    auto *editor = qobject_cast<NumberProvider*>(sender());
    emit  commitData(editor);
    emit  closeEditor(editor);
}

void NumberProviderDelegate::setMaxLimit(int value) {
    m_maxLimit = value;
}

void NumberProviderDelegate::setMinLimit(int value) {
    m_minLimit = value;
}

NumberProvider::Modes NumberProviderDelegate::inputModes() const {
    return m_inputModes;
}

void NumberProviderDelegate::setInputModes(
    const NumberProvider::Modes &value) {
    m_inputModes = value;
}
