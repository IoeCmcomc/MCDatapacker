#include "basecondition.h"

#include "mainwindow.h"

BaseCondition::BaseCondition() {
}

QJsonObject BaseCondition::toJson() const {
    return QJsonObject();
}

void BaseCondition::fromJson([[maybe_unused]] const QJsonObject &value) {
}
