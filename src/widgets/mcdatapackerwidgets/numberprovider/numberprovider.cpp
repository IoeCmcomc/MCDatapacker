#include "numberprovider.h"
#include "ui_numberprovider.h"

#include "stylesheetreapplier.h"

#include <QJsonObject>
#include <QDebug>
#include <QVector>
#include <QResizeEvent>

#include <cfloat>

NumberProvider::NumberProvider(QWidget *parent) :
    QFrame(parent), ui(new Ui::NumberProvider) {
    ui->setupUi(this);

    setIntegerOnly(m_integerOnly);
    setModes(Exact | Range | Binomial);

    connect(ui->minSpinBox, &QSpinBox::editingFinished,
            this, &NumberProvider::onMinMaxEdited);
    connect(ui->maxSpinBox, &QSpinBox::editingFinished,
            this, &NumberProvider::onMinMaxEdited);

    ui->inputTypeButton->installEventFilter(styleSheetReapplier);
}

NumberProvider::~NumberProvider() {
    delete ui;
}

void NumberProvider::resizeEvent(QResizeEvent *event) {
    const int eventWidth = event->size().width();

    ui->minLabel->setHidden(eventWidth < 200);
    ui->maxLabel->setHidden(eventWidth < 180);
    ui->numLabel->setHidden(eventWidth < 250);
    ui->probLabel->setHidden(eventWidth < 200);

    QFrame::resizeEvent(event);
}

void NumberProvider::mouseReleaseEvent(QMouseEvent *event) {
    auto cursorWidget = qApp->widgetAt(mapToGlobal(event->pos()));

    if (cursorWidget != nullptr
        && qobject_cast<QAbstractSpinBox *>(cursorWidget) == nullptr
        && qobject_cast<QToolButton *>(cursorWidget) == nullptr) {
        interpretText();
        emit editingFinished();
        return;
    }
    QFrame::mouseReleaseEvent(event);
}

void NumberProvider::onMinMaxEdited() {
    if (ui->minSpinBox->value() > ui->maxSpinBox->value()) {
        if (!(ui->minSpinBox->isUnset() || ui->maxSpinBox->isUnset())) {
            auto tmp = ui->minSpinBox->value();
            ui->minSpinBox->setValue(ui->maxSpinBox->value());
            ui->maxSpinBox->setValue(tmp);
        }
    } else if (ui->minSpinBox->value() == ui->maxSpinBox->value()) {
        setExactValue(ui->minSpinBox->value());
        if (ui->minSpinBox->isUnset())
            ui->spinBox->unset();
    }
}

void NumberProvider::fromJson(const QJsonValue &value) {
    /*unset(); */
    if (value.isDouble()) {
        ui->spinBox->setValue(m_integerOnly ? value.toInt() : value.toDouble());
        ui->stackedWidget->setCurrentIndex(0);
    } else if (value.isObject()) {
        auto obj  = value.toObject();
        auto type = obj.value(QLatin1String("type")).toString();
        if (type == QStringLiteral("minecraft:binomial")) {
            if (obj.contains(QStringLiteral("n")))
                ui->numSpinBox->setValue(obj.value(QStringLiteral("n")).toInt());
            if (obj.contains(QStringLiteral("p")))
                ui->probSpinBox->setValue(obj.value(QStringLiteral("p")).toInt());
            ui->stackedWidget->setCurrentIndex(2);
        } else {
            const bool hasMax = obj.contains(QStringLiteral("max"));
            if (obj.contains(QStringLiteral("min"))) {
                const auto min = obj[QStringLiteral("min")];
                setMinValue(m_integerOnly ? min.toInt() : min.toDouble(),
                            hasMax);
            } else {
                ui->minSpinBox->unset();
            }
            if (hasMax) {
                const auto max = obj[QStringLiteral("max")];
                setMaxValue(m_integerOnly ? max.toInt() : max.toDouble());
            } else {
                ui->maxSpinBox->unset();
            }
        }
    }
}

QJsonValue NumberProvider::toJson() {
    QJsonValue value;

    switch (ui->stackedWidget->currentIndex()) {
        case 0: { /* Exactly */
            value =
                (ui->spinBox->isUnset()) ? QJsonValue() : ui->spinBox->value();
            break;
        }

        case 1: { /* Range */
            QJsonObject root;
            if (!ui->minSpinBox->isUnset())
                root.insert(QLatin1String("min"), ui->minSpinBox->value());
            if (!ui->maxSpinBox->isUnset())
                root.insert(QLatin1String("max"), ui->maxSpinBox->value());
            value = root;
            break;
        }

        case 2: { /*Binomial */
            QJsonObject root;
            root.insert(QLatin1String("type"),
                        QLatin1String("minecraft:binomial"));
            root.insert("n", ui->numSpinBox->value());
            root.insert("p", ui->probSpinBox->value());
            value = root;
            break;
        }

        default: break;
    }
    return value;
}

NumberProvider::Modes NumberProvider::getModes() const {
    return m_modes;
}

void NumberProvider::setModes(const NumberProvider::Modes &value) {
    m_modes = value;
    setMenu();
    if (!m_modes.testFlag(Exact)) {
        ui->stackedWidget->setCurrentIndex(1);
        if (!m_modes.testFlag(Range))
            ui->stackedWidget->setCurrentIndex(2);
    }
    ui->inputTypeButton->setHidden(value != 0 && (value & (value - 1)) == 0);
    emit modesChanged();
}

void NumberProvider::setMenu() {
    m_menu.clear();

    if (m_modes.testFlag(Exact))
        m_menu.addAction(tr("Exactly"), this, [ = ]() {
            ui->stackedWidget->setCurrentIndex(0);
        });
    if (m_modes.testFlag(Range))
        m_menu.addAction(tr("Range"), this, [ = ]() {
            ui->stackedWidget->setCurrentIndex(1);
        });
    if (m_modes.testFlag(Binomial))
        m_menu.addAction(tr("Binomial"), this, [ = ]() {
            ui->stackedWidget->setCurrentIndex(2);
        });
    ui->inputTypeButton->setMenu(&m_menu);
    ui->inputTypeButton->setPopupMode(QToolButton::InstantPopup);
}

NumberProvider::Mode NumberProvider::currentMode() const {
    return m_currentMode;
}

void NumberProvider::setCurrentMode(const Mode &value) {
    m_currentMode = value;
    const static QMap<Mode, uint8_t> modeMap =
    { { Exact, 0 }, { Range, 1 }, { Binomial, 2 } };
    ui->stackedWidget->setCurrentIndex(modeMap.value(value));
    emit currentModeChanged();
}

bool NumberProvider::isIntegerOnly() const {
    return m_integerOnly;
}

void NumberProvider::setIntegerOnly(bool value) {
    m_integerOnly = value;
    ui->spinBox->setIntegerOnly(value);
    ui->minSpinBox->setIntegerOnly(value);
    ui->maxSpinBox->setIntegerOnly(value);
    emit integerOnlyChanged();
}

int NumberProvider::exactValue() const {
    return ui->spinBox->value();
}

void NumberProvider::setExactValue(const double value) {
    ui->spinBox->setValue(value);
    ui->stackedWidget->setCurrentIndex(0);
}

int NumberProvider::minValue() const {
    return ui->minSpinBox->value();
}

void NumberProvider::setMinValue(const double value, bool setMaxLater) {
    ui->minSpinBox->setValue(value);
    ui->stackedWidget->setCurrentIndex(1);
    if (!setMaxLater) {
        onMinMaxEdited();
    }
}

int NumberProvider::maxValue() const {
    return ui->maxSpinBox->value();
}

void NumberProvider::setMaxValue(const double value) {
    ui->maxSpinBox->setValue(value);
    ui->stackedWidget->setCurrentIndex(1);
    onMinMaxEdited();
}

int NumberProvider::minLimit() const {
    return ui->minSpinBox->minimum();
}

void NumberProvider::setMinLimit(const double min) {
    ui->spinBox->setMinimum(min);
    ui->minSpinBox->setMinimum(min);
    ui->maxSpinBox->setMinimum(min);
    emit minLimitChanged();
}

int NumberProvider::maxLimit() const {
    return ui->maxSpinBox->maximum();
}

void NumberProvider::setMaxLimit(const double max) {
    ui->spinBox->setMaximum(max);
    ui->minSpinBox->setMaximum(max);
    ui->maxSpinBox->setMaximum(max);
    emit maxLimitChanged();
}

/*! Set the minimum limit to the minimum  value */
void NumberProvider::minimizeMinLimit() {
    setMinLimit(m_integerOnly ? INT_MIN : DBL_MIN);
}

bool NumberProvider::isCurrentlyUnset() const {
    switch (ui->stackedWidget->currentIndex()) {
        case 0: /* Exactly */
            return ui->spinBox->isUnset();

        case 1: /* Range */
            return (ui->minSpinBox->isUnset() && ui->maxSpinBox->isUnset());

        case 2: /*Binomial */
            return true;

        default:
            return false;
    }
}

void NumberProvider::unset() const {
    ui->spinBox->unset();
    ui->minSpinBox->unset();
    ui->maxSpinBox->unset();
}

void NumberProvider::interpretText() {
    ui->spinBox->interpretText();
    ui->minSpinBox->interpretText();
    ui->maxSpinBox->interpretText();
    ui->numSpinBox->interpretText();
    ui->probSpinBox->interpretText();
}
