#ifndef OPTIONALSPINBOX_H
#define OPTIONALSPINBOX_H

#include <QSpinBox>

class OptionalSpinBox : public QDoubleSpinBox {
public:
    explicit OptionalSpinBox(QWidget *parent = nullptr);

    bool isUnset() const;
    void unset();

    bool isIntegerOnly() const;
    void setIntegerOnly(bool newIntegerOnly);

    void setValue(double value);

protected:
    QValidator::State validate(QString &text, int &pos) const final;
    double valueFromText(const QString &text) const final;
    QString textFromValue(double value) const final;
    void fixup(QString &input) const final;
    QSize sizeHint() const final;
    QSize minimumSizeHint() const final;

private:
    bool mutable m_isUnset = true;
    bool m_integerOnly     = true;
};

#endif /* OPTIONALSPINBOX_H */
