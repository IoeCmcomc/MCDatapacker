#include "extendednumericinput.h"
#include "ui_extendednumericinput.h"

#include <QMenu>
#include <QJsonObject>

ExtendedNumericInput::ExtendedNumericInput(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ExtendedNumericInput) {
    ui->setupUi(this);

    QMenu *menu = new QMenu(this);
    menu->addAction(tr("Exactly"), this, [ = ]() {
        ui->stackedWidget->setCurrentIndex(0);
    });
    menu->addAction(tr("Range"), this, [ = ]() {
        ui->stackedWidget->setCurrentIndex(1);
    });
    menu->addAction(tr("Biomimal"), this, [ = ]() {
        ui->stackedWidget->setCurrentIndex(2);
    });
    ui->inputTypeButton->setMenu(menu);
    ui->inputTypeButton->setPopupMode(QToolButton::InstantPopup);
}

ExtendedNumericInput::~ExtendedNumericInput() {
    delete ui;
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
