#include "extendednumericinput.h"
#include "ui_extendednumericinput.h"

#include <QJsonObject>
#include <QDebug>
#include <QVector>
#include <QResizeEvent>

ExtendedNumericInput::ExtendedNumericInput(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ExtendedNumericInput) {
    ui->setupUi(this);

    setTypes(Exact | Range | Biomimal);

    connect(ui->minSpinBox, &QSpinBox::editingFinished,
            this, &ExtendedNumericInput::onMinMaxEdited);
    connect(ui->maxSpinBox, &QSpinBox::editingFinished,
            this, &ExtendedNumericInput::onMinMaxEdited);
}

ExtendedNumericInput::~ExtendedNumericInput() {
    delete ui;
}

void ExtendedNumericInput::resizeEvent(QResizeEvent *event) {
    int eventWidth = event->size().width();

    ui->minLabel->setHidden(eventWidth < 200);
    ui->maxLabel->setHidden(eventWidth < 180);
    ui->numLabel->setHidden(eventWidth < 250);
    ui->probLabel->setHidden(eventWidth < 200);

    QFrame::resizeEvent(event);
}

void ExtendedNumericInput::mouseReleaseEvent(QMouseEvent *event) {
    auto cursorWidget = qApp->widgetAt(mapToGlobal(event->pos()));

    /*qDebug() << "cursorWidget:" << cursorWidget; */
    if (cursorWidget != nullptr
        && qobject_cast<QAbstractSpinBox*>(cursorWidget) == nullptr
        && qobject_cast<QToolButton*>(cursorWidget) == nullptr) {
        interpretText();
        emit editingFinished();
        return;
    }
    QFrame::mouseReleaseEvent(event);
}

void ExtendedNumericInput::onMinMaxEdited() {
    if (ui->minSpinBox->value() > ui->maxSpinBox->value()) {
        auto tmp = ui->minSpinBox->value();
        ui->minSpinBox->setValue(ui->maxSpinBox->value());
        ui->maxSpinBox->setValue(tmp);
    }
}

void ExtendedNumericInput::fromJson(const QJsonValue &value) {
    if (value.isDouble()) {
        ui->spinBox->setValue(value.toInt());
        ui->stackedWidget->setCurrentIndex(0);
    } else if (value.isObject()) {
        auto obj  = value.toObject();
        auto type = obj[QStringLiteral("type")].toString();
        if (type == QStringLiteral("minecraft:binomial")) {
            ui->numSpinBox->setValue(obj.value(QStringLiteral("n")).toInt());
            ui->probSpinBox->setValue(obj.value(QStringLiteral("p")).toInt());
            ui->stackedWidget->setCurrentIndex(2);
        } else if (obj.contains(QStringLiteral("min")) &&
                   obj.contains(QStringLiteral("max"))) {
            ui->minSpinBox->setValue(obj.value(QStringLiteral("min")).toInt());
            ui->maxSpinBox->setValue(obj.value(QStringLiteral("max")).toInt());
            ui->stackedWidget->setCurrentIndex(1);
        }
    }
}

QJsonValue ExtendedNumericInput::toJson() {
    QJsonValue value;

    switch (ui->stackedWidget->currentIndex()) {
    case 0: { /* Exactly */
        value = ui->spinBox->value();
        break;
    }

    case 1: { /* Range */
        QJsonObject rolls;
        rolls.insert(QStringLiteral("min"), ui->minSpinBox->value());
        rolls.insert(QStringLiteral("max"), ui->maxSpinBox->value());
        value = rolls;
        break;
    }

    case 2: { /*Biomimal */
        QJsonObject rolls;
        rolls.insert(QStringLiteral("type"),
                     QStringLiteral("minecraft:binomial"));
        rolls.insert(QStringLiteral("n"), ui->numSpinBox->value());
        rolls.insert(QStringLiteral("p"), ui->probSpinBox->value());
        value = rolls;
        break;
    }

    default: break;
    }
    return value;
}

ExtendedNumericInput::Types ExtendedNumericInput::getTypes() const {
    return types;
}

void ExtendedNumericInput::setTypes(const ExtendedNumericInput::Types &value) {
    types = value;
    /*qDebug() << value; */
    setMenu();
    /*qDebug() << types.testFlag(Exact) << types.testFlag(Range) << types.testFlag(Biomimal); */
    if (!types.testFlag(Exact)) {
        ui->stackedWidget->setCurrentIndex(1);
        if (!types.testFlag(Range))
            ui->stackedWidget->setCurrentIndex(2);
    }
}

void ExtendedNumericInput::setMenu() {
    typeMenu.clear();

    if (types.testFlag(Exact))
        typeMenu.addAction(tr("Exactly"), this, [ = ]() {
            ui->stackedWidget->setCurrentIndex(0);
        });
    if (types.testFlag(Range))
        typeMenu.addAction(tr("Range"), this, [ = ]() {
            ui->stackedWidget->setCurrentIndex(1);
        });
    if (types.testFlag(Biomimal))
        typeMenu.addAction(tr("Biomimal"), this, [ = ]() {
            ui->stackedWidget->setCurrentIndex(2);
        });
    ui->inputTypeButton->setMenu(&typeMenu);
    ui->inputTypeButton->setPopupMode(QToolButton::InstantPopup);
}

ExtendedNumericInput::Type ExtendedNumericInput::getCurrentType() const {
    return currentType;
}

void ExtendedNumericInput::setCurrentType(const Type &value) {
    currentType = value;
    const QVector<Type> typeVec = { Exact, Range, Biomimal };
    ui->stackedWidget->setCurrentIndex(typeVec.indexOf(value));
}

int ExtendedNumericInput::getExactly() const {
    return ui->spinBox->value();
}

void ExtendedNumericInput::setExactly(const int value) {
    ui->spinBox->setValue(value);
    ui->stackedWidget->setCurrentIndex(0);
}

int ExtendedNumericInput::getMinimum() const {
    return ui->minSpinBox->value();
}

void ExtendedNumericInput::setMinimum(const int value) {
    ui->minSpinBox->setValue(value);
    ui->stackedWidget->setCurrentIndex(1);
    onMinMaxEdited();
}

int ExtendedNumericInput::getMaximum() const {
    return ui->maxSpinBox->value();
}

void ExtendedNumericInput::setMaximum(const int value) {
    ui->maxSpinBox->setValue(value);
    ui->stackedWidget->setCurrentIndex(1);
    onMinMaxEdited();
}

void ExtendedNumericInput::interpretText() {
    ui->spinBox->interpretText();
    ui->minSpinBox->interpretText();
    ui->maxSpinBox->interpretText();
    ui->numSpinBox->interpretText();
    ui->probSpinBox->interpretText();
}
