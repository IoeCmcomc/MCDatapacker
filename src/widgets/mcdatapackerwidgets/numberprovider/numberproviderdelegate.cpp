#include "numberproviderdelegate.h"

#include "extendedtablewidget.h"

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
        if (!index.data(ExtendedTableWidget::NumberProviderRole).isNull()) {
            const QJsonValue &&value = index.data(
                ExtendedTableWidget::NumberProviderRole).toJsonValue();

            auto opt = option;
            initStyleOption(&opt, index);

            opt.text = textRepr(value);

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
    if (!index.data(ExtendedTableWidget::NumberProviderRole).isNull()) {
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
    if (!index.data(ExtendedTableWidget::NumberProviderRole).isNull()) {
        QJsonValue jsonVal = index.data(
            ExtendedTableWidget::NumberProviderRole).toJsonValue();
        auto *numEditor = qobject_cast<NumberProvider*>(editor);
        numEditor->fromJson(jsonVal);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void NumberProviderDelegate::setModelData(QWidget *editor,
                                          QAbstractItemModel *model,
                                          const QModelIndex &index) const {
    if (!index.data(ExtendedTableWidget::NumberProviderRole).isNull()) {
        auto *numEditor = qobject_cast<NumberProvider*>(editor);
        numEditor->interpretText();
        model->setData(index,
                       numEditor->toJson(),
                       ExtendedTableWidget::NumberProviderRole);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

QSize NumberProviderDelegate::sizeHint(const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const {
    auto hint = QStyledItemDelegate::sizeHint(option, index);

    const QFontMetrics fm(option.font);
    const QString    &&text =
        textRepr(index.data(ExtendedRole::NumberProviderRole).toJsonValue());

    hint = hint.expandedTo(QSize(qMax(fm.horizontalAdvance(text), 160),
                                 hint.height()));

    return hint;
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

QString NumberProviderDelegate::textRepr(const QJsonValue &value) const {
    if (value.isDouble()) {
        const int exactInt = value.toInt();
        return QString::number(exactInt);
    } else if (value.isObject()) {
        const auto    &&obj  = value.toObject();
        const QString &&type = obj[QLatin1String("type")].toString();
        if (type == QLatin1String("minecraft:binomial")) {
            int    num  = obj.value(QLatin1String("n")).toInt();
            double prob = obj.value(QLatin1String("p")).toInt();
            return QString("n: %1; p: %2").arg(num).arg(prob);
        } else {
            const QString &&min = (obj.contains(QLatin1String("min")))
                                      ? QString::number(
                obj.value(QLatin1String("min")).toInt())
                                      : QString();
            const QString &&max = (obj.contains(QLatin1String("max")))
                                      ? QString::number(
                obj.value(QLatin1String("max")).toInt())
                                      : QString();
            return QString("%1..%2").arg(min, max);
        }
    } else {
        return QString();
    }
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
