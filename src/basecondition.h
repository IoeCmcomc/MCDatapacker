#ifndef BASECONDITION_H
#define BASECONDITION_H

#include "numberprovider.h"
#include "vieweventfilter.h"
#include "modelfunctions.h"

#include <QDebug>
#include <QStandardItemModel>

class BaseCondition
{
public:
    explicit BaseCondition() = default;

    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject &value);

protected:
    ViewEventFilter viewFilter;
};

#endif /* BASECONDITION_H */
