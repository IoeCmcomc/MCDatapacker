#ifndef BASECONDITION_H
#define BASECONDITION_H

#include "vieweventfilter.h"
#include "modelfunctions.h"

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
