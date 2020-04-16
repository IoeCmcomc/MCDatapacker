#ifndef OPTIONALSPINBOX_H
#define OPTIONALSPINBOX_H

#include <QSpinBox>

class OptionalSpinBox : public QSpinBox
{
public:
    OptionalSpinBox(QWidget *parent = nullptr);

    bool isUnset() const;
    void unset();

protected:
    QValidator::State validate(QString &text, int &pos) const override;
    int valueFromText(const QString &text) const override;
    QString textFromValue(int value) const override;
    void fixup(QString &input) const override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    QString unsetDisplayStr = "-";

private:
    bool mutable m_isUnset = true;
};

#endif /* OPTIONALSPINBOX_H */
