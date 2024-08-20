#ifndef MODELFUNCTIONS_H
#define MODELFUNCTIONS_H

#include "vieweventfilter.h"

#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
class QComboBox;
class QTableWidget;
class QTableWidgetItem;
class QAbstractItemDelegate;
class QTableView;
QT_END_NAMESPACE

void initModelView(QStandardItemModel &model, QTableView *tableView,
                   std::initializer_list<QStandardItem *> headers,
                   ViewEventFilter &eventFilter,
                   QAbstractItemDelegate *delegate = nullptr);
void initComboModelView(const QString &infoType,
                        QStandardItemModel &model, QComboBox *combo,
                        bool optional = true, bool append        = true,
                        bool asTag    = false, bool forceKeyName = false);

void initComboModelViewFromRegistry(const QString &registry,
                                    QStandardItemModel &model, QComboBox *combo,
                                    bool optional = true, bool append = true);

void setComboValueFrom(QComboBox *combo, const QVariant &vari,
                       int role = Qt:: UserRole + 1);

void appendRowToTableWidget(QTableWidget *table,
                            std::initializer_list<QTableWidgetItem *> items);
void appendRowToModel(QStandardItemModel &model, const QString &text,
                      const QVariant &data, int role = Qt::UserRole + 1);

void hideComboRow(QComboBox *comboBox, const int row);
void setComboRowHidden(QComboBox *comboBox, const int row, bool hidden);


const QString deletiveToolTip = QStringLiteral(QT_TRANSLATE_NOOP("toolTip",
                                                                 "Right click this row to delete."));

#endif /* MODELFUNCTIONS_H */
