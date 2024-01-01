#include "optionalspinbox.h"

#include <QDebug>
#include <QLineEdit>

const QString &&unsetDisplayStr = QStringLiteral("-");

OptionalSpinBox::OptionalSpinBox(QWidget *parent) : QDoubleSpinBox(parent) {
    setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
    setIntegerOnly(m_integerOnly);
}

QValidator::State OptionalSpinBox::validate(QString &text, int &pos) const {
    if (text == unsetDisplayStr) {
        return QValidator::Acceptable;
    } else {
        auto state = QDoubleSpinBox::validate(text, pos);
        if (!m_integerOnly) return state;

        if (state == QValidator::Invalid) return QValidator::Invalid;

        bool ok = false;
        if ([[maybe_unused]] int _ = text.toInt(&ok); !ok)
            return QValidator::Invalid;

        return state;
    }
}

double OptionalSpinBox::valueFromText(const QString &text) const {
    /*qDebug() << "valueFromText" << text << isUnset << minimum() << maximum(); */
    if (text == unsetDisplayStr) {
        m_isUnset = true;
        return qMax(0., minimum());
    } else {
        m_isUnset = false;
        const double value = QDoubleSpinBox::valueFromText(text);
        return m_integerOnly ? (int)value : value;
    }
}

QString OptionalSpinBox::textFromValue(double value) const {
    /*qDebug() << "textFromValue" << value << isUnset << minimum() << maximum(); */
    if (m_isUnset && (value == qMax(0., minimum())))
        return unsetDisplayStr;
    else {
        m_isUnset = false;
        if (m_integerOnly) {
            return QString::number((int)value);
        } else {
            QString &&text = QDoubleSpinBox::textFromValue(value);
            if (int decSepPos = text.indexOf(locale().decimalPoint());
                decSepPos != -1) {
                if (int i = text.indexOf('0', decSepPos); i != -1) {
                    // Trims trailing zeros in the decimal parts
                    // Trims the decimal point if necessary
                    return text.left(i - (i - decSepPos == 1));
                }
            }
            return text;
        }
    }
}

void OptionalSpinBox::fixup(QString &input) const {
    if (input.isEmpty())
        input = unsetDisplayStr;
    else
        QDoubleSpinBox::fixup(input);
}

QSize OptionalSpinBox::sizeHint() const {
    bool  isUnsetBefore = m_isUnset;
    QSize size          = QDoubleSpinBox::sizeHint();

    m_isUnset = isUnsetBefore;
    return size;
}

QSize OptionalSpinBox::minimumSizeHint() const {
    bool  isUnsetBefore = m_isUnset;
    QSize size          = QDoubleSpinBox::minimumSizeHint();

    m_isUnset = isUnsetBefore;
    return size;
}

bool OptionalSpinBox::isIntegerOnly() const {
    return m_integerOnly;
}

void OptionalSpinBox::setIntegerOnly(bool newIntegerOnly) {
    m_integerOnly = newIntegerOnly;
    if (newIntegerOnly) {
        setDecimals(0);
    } else {
        setDecimals(std::numeric_limits<float>::digits10);
    }
}

void OptionalSpinBox::setValue(double value) {
    m_isUnset = false;
    QDoubleSpinBox::setValue(value);
}

bool OptionalSpinBox::isUnset() const {
    return m_isUnset;
}

void OptionalSpinBox::unset() {
    m_isUnset = true;
    clear();
    setValue(qMax(0., minimum()));
}
