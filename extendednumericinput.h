#ifndef EXTENDEDNUMERICINPUT_H
#define EXTENDEDNUMERICINPUT_H

#include <QFrame>
#include <QMenu>
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

    enum Type {
        Exact    = 1,
        Range    = 2,
        Biomimal = 4
    };
    Q_DECLARE_FLAGS(Types, Type)

    void fromJson(const QJsonValue &value);
    QJsonValue toJson();

    Types getTypes() const;
    void setTypes(const Types &value);
    Type getCurrentType() const;
    void setCurrentType(const Type &value);

    int getExactly() const;
    void setExactly(const int value);
    int getMinimum() const;
    void setMinimum(const int value);
    int getMaximum() const;
    void setMaximum(const int value);

    void interpretText();

signals:
    void editingFinished();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onMinMaxEdited();

private:
    Ui::ExtendedNumericInput *ui;
    QMenu typeMenu;
    Types types;
    Type currentType;

    void setMenu();
    void swapMinMax();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ExtendedNumericInput::Types)

#endif /* EXTENDEDNUMERICINPUT_H */
