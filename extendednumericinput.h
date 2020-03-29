#ifndef EXTENDEDNUMERICINPUT_H
#define EXTENDEDNUMERICINPUT_H

#include <QFrame>
#include <QJsonValue>

namespace Ui {
    class ExtendedNumericInput;
}

class ExtendedNumericInput : public QFrame
{
    Q_OBJECT

public:
    explicit ExtendedNumericInput(QWidget *parent = nullptr);
    ~ExtendedNumericInput();

    void fromJson(const QJsonValue &value);
    QJsonValue toJson();

private:
    Ui::ExtendedNumericInput *ui;
};

#endif /* EXTENDEDNUMERICINPUT_H */
