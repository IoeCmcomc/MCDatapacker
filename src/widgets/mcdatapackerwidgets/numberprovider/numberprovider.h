#ifndef NUMBERPROVIDER_H
#define NUMBERPROVIDER_H

#include <QFrame>
#include <QMenu>
#include <QJsonValue>

namespace Ui {
    class NumberProvider;
}

class NumberProvider : public QFrame {
    Q_OBJECT

    Q_PROPERTY(Modes modes READ getModes WRITE setModes NOTIFY modesChanged)
    Q_PROPERTY(
        Mode currentMode READ currentMode WRITE setCurrentMode NOTIFY currentModeChanged)
    Q_PROPERTY(
        double minLimit READ minLimit WRITE setMinLimit NOTIFY minLimitChanged)
    Q_PROPERTY(
        double maxLimit READ maxLimit WRITE setMaxLimit NOTIFY maxLimitChanged)
    Q_PROPERTY(bool isUnset READ isCurrentlyUnset STORED false)
    Q_PROPERTY(
        bool isIntegerOnly READ isIntegerOnly WRITE setIntegerOnly NOTIFY integerOnlyChanged)

public:
    explicit NumberProvider(QWidget *parent = nullptr);
    ~NumberProvider();

    enum Mode {
        Exact         = 1,
        Range         = 2,
        ExactAndRange = Exact | Range,
        Binomial      = 4,
    };
    Q_ENUM(Mode);
    Q_DECLARE_FLAGS(Modes, Mode)
    Q_FLAG(Modes)

    void fromJson(const QJsonValue &value);
    QJsonValue toJson();

    Modes getModes() const;
    void setModes(const Modes &value);
    Mode currentMode() const;
    void setCurrentMode(const Mode &value);
    bool isIntegerOnly() const;
    void setIntegerOnly(bool value);

    int exactValue() const;
    void setExactValue(const double value);
    int minValue() const;
    void setMinValue(const double value, bool setMaxLater = false);
    int maxValue() const;
    void setMaxValue(const double value);

    int minLimit() const;
    void setMinLimit(const double min);
    int maxLimit() const;
    void setMaxLimit(const double max);

    void minimizeMinLimit();

    bool isCurrentlyUnset() const;
    void unset() const;

    void interpretText();

signals:
    void editingFinished();
    void modesChanged();
    void currentModeChanged();
    void minLimitChanged();
    void maxLimitChanged();
    void unsetChanged();
    void integerOnlyChanged();

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
    bool m_integerOnly = true;

    void setMenu();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(NumberProvider::Modes)

#endif /* NUMBERPROVIDER_H */
