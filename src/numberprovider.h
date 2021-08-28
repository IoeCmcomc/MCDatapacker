#ifndef NUMBERPROVIDER_H
#define NUMBERPROVIDER_H

#include <QFrame>
#include <QMenu>
#include <QJsonValue>

namespace Ui {
    class NumberProvider;
}

class NumberProvider : public QFrame
{
    Q_OBJECT

public:
    explicit NumberProvider(QWidget *parent = nullptr);
    ~NumberProvider();

    enum Mode {
        Exact         = 1,
        Range         = 2,
        ExactAndRange = Exact | Range,
        Binomial      = 4,
    };
    Q_DECLARE_FLAGS(Modes, Mode)

    void fromJson(const QJsonValue &value);
    QJsonValue toJson();

    Modes getModes() const;
    void setModes(const Modes &value);
    Mode currentMode() const;
    void setCurrentMode(const Mode &value);

    int exactValue() const;
    void setExactValue(const int value);
    int minValue() const;
    void setMinValue(const int value);
    int maxValue() const;
    void setMaxValue(const int value);

    void setMinLimit(const int &min);
    void setMaxLimit(const int &max);

    void minimizeMinLimit();

    bool isCurrentlyUnset() const;
    void unset() const;

    void interpretText();

signals:
    void editingFinished();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onMinMaxEdited();

private:
    Ui::NumberProvider *ui;
    QMenu m_menu;
    Modes m_modes;
    Mode m_currentMode;

    void setMenu();
    void swapMinMax();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(NumberProvider::Modes)

#endif /* NUMBERPROVIDER_H */
