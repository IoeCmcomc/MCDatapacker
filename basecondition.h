#ifndef BASECONDITION_H
#define BASECONDITION_H

#include "numericinput.h"
#include "vieweventfilter.h"

#include <QStandardItemModel>
#include <QTableView>
#include <QComboBox>
#include <QDebug>

class BaseCondition
{
public:
    explicit BaseCondition();

    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject &value);

protected:
    const QString deletiveToolTip = QT_TRANSLATE_NOOP("toolTip",
                                                      "Right click this row to delete.");
    ViewEventFilter viewFilter;

    virtual void initModelView(QStandardItemModel &model, QTableView *tableView,
                               std::initializer_list<QStandardItem*> headers,
                               QAbstractItemDelegate *delegate = nullptr);
    virtual void initComboModelView(const QString &infoType,
                                    QStandardItemModel &model, QComboBox *combo,
                                    bool optional = true);
    virtual void setupComboFrom(QComboBox *combo, const QVariant &vari,
                                int role = Qt:: UserRole + 1);
};

#endif /* BASECONDITION_H */
