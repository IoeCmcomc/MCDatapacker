#include "numericinput.h"
#include "ui_numericinput.h"

#include <QJsonObject>
#include <QDebug>
#include <QVector>
#include <QResizeEvent>

NumericInput::NumericInput(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::NumericInput) {
    ui->setupUi(this);

    setTypes(Exact | Range | Binomial);

    connect(ui->minSpinBox, &QSpinBox::editingFinished,
            this, &NumericInput::onMinMaxEdited);
    connect(ui->maxSpinBox, &QSpinBox::editingFinished,
            this, &NumericInput::onMinMaxEdited);
}

NumericInput::~NumericInput() {
    delete ui;
}

void NumericInput::resizeEvent(QResizeEvent *event) {
    int eventWidth = event->size().width();

    ui->minLabel->setHidden(eventWidth < 200);
    ui->maxLabel->setHidden(eventWidth < 180);
    ui->numLabel->setHidden(eventWidth < 250);
    ui->probLabel->setHidden(eventWidth < 200);

    QFrame::resizeEvent(event);
}

void NumericInput::mouseReleaseEvent(QMouseEvent *event) {
    auto cursorWidget = qApp->widgetAt(mapToGlobal(event->pos()));

    if (cursorWidget != nullptr
        && qobject_cast<QAbstractSpinBox*>(cursorWidget) == nullptr
        && qobject_cast<QToolButton*>(cursorWidget) == nullptr) {
        interpretText();
        emit editingFinished();
        return;
    }
    QFrame::mouseReleaseEvent(event);
}

void NumericInput::onMinMaxEdited() {
    if (ui->minSpinBox->value() > ui->maxSpinBox->value()) {
        if (!(ui->minSpinBox->isUnset() || ui->maxSpinBox->isUnset())) {
            auto tmp = ui->minSpinBox->value();
            ui->minSpinBox->setValue(ui->maxSpinBox->value());
            ui->maxSpinBox->setValue(tmp);
        }
    } else if (ui->minSpinBox->value() == ui->maxSpinBox->value()) {
        setExactly(ui->minSpinBox->value());
        if (ui->minSpinBox->isUnset())
            ui->spinBox->unset();
    }
}

void NumericInput::fromJson(const QJsonValue &value) {
    if (value.isDouble()) {
        ui->spinBox->setValue(value.toInt());
        ui->stackedWidget->setCurrentIndex(0);
    } else if (value.isObject()) {
        auto obj  = value.toObject();
        auto type = obj[QStringLiteral("type")].toString();
        if (type == QStringLiteral("minecraft:binomial")) {
            if (obj.contains(QStringLiteral("n")))
                ui->numSpinBox->setValue(obj.value(QStringLiteral("n")).toInt());
            if (obj.contains(QStringLiteral("p")))
                ui->probSpinBox->setValue(obj.value(QStringLiteral("p")).toInt());
            ui->stackedWidget->setCurrentIndex(2);
        } else {
            if (obj.contains(QStringLiteral("min")))
                setMinimum(obj.value(QStringLiteral("min")).toInt());
            else
                ui->minSpinBox->unset();
            if (obj.contains(QStringLiteral("max")))
                setMaximum(obj.value(QStringLiteral("max")).toInt());
            else
                ui->maxSpinBox->unset();
        }
    }
}

QJsonValue NumericInput::toJson() {
    QJsonValue value;

    switch (ui->stackedWidget->currentIndex()) {
    case 0: { /* Exactly */
        value =
            (ui->spinBox->isUnset()) ? QJsonValue() : ui->spinBox->value();
        break;
    }

    case 1: { /* Range */
        QJsonObject root;
/*
          qDebug() << ui->minSpinBox->isUnset() <<
              ui->maxSpinBox->isUnset();
 */
        if (!ui->minSpinBox->isUnset())
            root.insert(QStringLiteral("min"), ui->minSpinBox->value());
        if (!ui->maxSpinBox->isUnset())
            root.insert(QStringLiteral("max"), ui->maxSpinBox->value());
        value = root;
        break;
    }

    case 2: { /*Binomial */
        QJsonObject root;
        root.insert(QStringLiteral("type"),
                    QStringLiteral("minecraft:binomial"));
        root.insert(QStringLiteral("n"), ui->numSpinBox->value());
        root.insert(QStringLiteral("p"), ui->probSpinBox->value());
        value = root;
        break;
    }

    default: break;
    }
    return value;
}

NumericInput::Types NumericInput::getTypes() const {
    return types;
}

void NumericInput::setTypes(const NumericInput::Types &value) {
    types = value;
    setMenu();
    if (!types.testFlag(Exact)) {
        ui->stackedWidget->setCurrentIndex(1);
        if (!types.testFlag(Range))
            ui->stackedWidget->setCurrentIndex(2);
    }
    ui->inputTypeButton->setHidden(value != 0 && (value & (value - 1)) == 0);
}

void NumericInput::setMenu() {
    typeMenu.clear();

    if (types.testFlag(Exact))
        typeMenu.addAction(tr("Exactly"), this, [ = ]() {
            ui->stackedWidget->setCurrentIndex(0);
        });
    if (types.testFlag(Range))
        typeMenu.addAction(tr("Range"), this, [ = ]() {
            ui->stackedWidget->setCurrentIndex(1);
        });
    if (types.testFlag(Binomial))
        typeMenu.addAction(tr("Binomial"), this, [ = ]() {
            ui->stackedWidget->setCurrentIndex(2);
        });
    ui->inputTypeButton->setMenu(&typeMenu);
    ui->inputTypeButton->setPopupMode(QToolButton::InstantPopup);
}

NumericInput::Type NumericInput::getCurrentType() const {
    return currentType;
}

void NumericInput::setCurrentType(const Type &value) {
    currentType = value;
    const QVector<Type> typeVec = { Exact, Range, Binomial };
    ui->stackedWidget->setCurrentIndex(typeVec.indexOf(value));
}

int NumericInput::getExactly() const {
    return ui->spinBox->value();
}

void NumericInput::setExactly(const int value) {
    ui->spinBox->setValue(value);
    ui->stackedWidget->setCurrentIndex(0);
}

int NumericInput::getMinimum() const {
    return ui->minSpinBox->value();
}

void NumericInput::setMinimum(const int value) {
    ui->minSpinBox->setValue(value);
    ui->stackedWidget->setCurrentIndex(1);
    onMinMaxEdited();
}

int NumericInput::getMaximum() const {
    return ui->maxSpinBox->value();
}

void NumericInput::setMaximum(const int value) {
    ui->maxSpinBox->setValue(value);
    ui->stackedWidget->setCurrentIndex(1);
    onMinMaxEdited();
}

void NumericInput::setExactMinimum(const int &min) {
    ui->spinBox->setMinimum(min);
}

void NumericInput::setExactMaximum(const int &max) {
    ui->spinBox->setMaximum(max);
}

void NumericInput::setRangeMinimum(const int &min) {
    ui->minSpinBox->setMinimum(min);
    ui->maxSpinBox->setMinimum(min);
}

void NumericInput::setRangeMaximum(const int &max) {
    ui->minSpinBox->setMaximum(max);
    ui->maxSpinBox->setMaximum(max);
}

void NumericInput::setGeneralMinimum(const int &min) {
    setExactMinimum(min);
    setRangeMinimum(min);
}

void NumericInput::setGeneralMaximum(const int &max) {
    setExactMaximum(max);
    setRangeMaximum(max);
}

bool NumericInput::isCurrentlyUnset() const {
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

void NumericInput::unset() const {
    ui->spinBox->unset();
    ui->minSpinBox->unset();
    ui->maxSpinBox->unset();
}

void NumericInput::interpretText() {
    ui->spinBox->interpretText();
    ui->minSpinBox->interpretText();
    ui->maxSpinBox->interpretText();
    ui->numSpinBox->interpretText();
    ui->probSpinBox->interpretText();
}

void NumericInput::retranslate() {
    ui->retranslateUi(this);
}
