#include "optionalspinbox.h"

#include <QDebug>
#include <QLineEdit>

OptionalSpinBox::OptionalSpinBox(QWidget *parent) : QSpinBox(parent) {
}

QValidator::State OptionalSpinBox::validate(QString &text, int &pos) const {
    if (text == m_unsetDisplayStr)
        return QValidator::Acceptable;
    else
        return QSpinBox::validate(text, pos);
}

int OptionalSpinBox::valueFromText(const QString &text) const {
    /*qDebug() << "valueFromText" << text << isUnset << minimum() << maximum(); */
    if (text == m_unsetDisplayStr) {
        m_isUnset = true;
        return qMax(0, minimum());
    } else {
        m_isUnset = false;
        return QSpinBox::valueFromText(text);
    }
}

QString OptionalSpinBox::textFromValue(int value) const {
    /*qDebug() << "textFromValue" << value << isUnset << minimum() << maximum(); */
    if (m_isUnset && (value == qMax(0, minimum())))
        return m_unsetDisplayStr;
    else {
        m_isUnset = false;
        return QSpinBox::textFromValue(value);
    }
}

void OptionalSpinBox::fixup(QString &input) const {
    if (input.isEmpty())
        input = m_unsetDisplayStr;
    else
        QSpinBox::fixup(input);
}

QSize OptionalSpinBox::sizeHint() const {
    bool  isUnsetBefore = m_isUnset;
    QSize size          = QSpinBox::sizeHint();

    m_isUnset = isUnsetBefore;
    return size;
}

QSize OptionalSpinBox::minimumSizeHint() const {
    bool  isUnsetBefore = m_isUnset;
    QSize size          = QSpinBox::minimumSizeHint();

    m_isUnset = isUnsetBefore;
    return size;
}

bool OptionalSpinBox::isUnset() const {
    return m_isUnset;
}

void OptionalSpinBox::unset() {
    m_isUnset = true;
    clear();
    setValue(qMax(0, minimum()));
}
