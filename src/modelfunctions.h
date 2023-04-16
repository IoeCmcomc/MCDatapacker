#ifndef MODELFUNCTIONS_H
#define MODELFUNCTIONS_H

#include "vieweventfilter.h"

#include <QStandardItemModel>
#include <QTableView>
#include <QComboBox>
#include <QTableWidget>

void initModelView(QStandardItemModel &model, QTableView *tableView,
                   std::initializer_list<QStandardItem *> headers,
                   ViewEventFilter &eventFilter,
                   QAbstractItemDelegate *delegate = nullptr);
void initComboModelView(const QString &infoType,
                        QStandardItemModel &model, QComboBox *combo,
                        bool optional = true, bool append = true,
                        bool asTag    = false);

void initComboModelViewFromRegistry(const QString &registry,
                                    QStandardItemModel &model, QComboBox *combo,
                                    bool optional = true, bool append = true);

void setComboValueFrom(QComboBox *combo, const QVariant &vari,
                       int role = Qt:: UserRole + 1);

void appendRowToTableWidget(QTableWidget *table,
                            std::initializer_list<QTableWidgetItem *> items);
void appendRowToModel(QStandardItemModel &model, const QString &text,
                      const QVariant &data, int role = Qt::UserRole + 1);

const QString deletiveToolTip = QStringLiteral(QT_TRANSLATE_NOOP("toolTip",
                                                                 "Right click this row to delete."));

#endif /* MODELFUNCTIONS_H */
