#include "basecondition.h"

#include <QJsonObject>

QJsonObject BaseCondition::toJson() const {
    return QJsonObject();
}

void BaseCondition::fromJson([[maybe_unused]] const QJsonObject &value) {
}
