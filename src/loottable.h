#ifndef LOOTTABLE_H
#define LOOTTABLE_H

#include <QTabWidget>

namespace Ui {
    class LootTable;
}

/*!
 * \class LootTable
 * \brief A widget which visually and logically represents a loot table.
 */
class LootTable : public QTabWidget {
    Q_OBJECT

public:
    explicit LootTable(QWidget *parent = nullptr);
    ~LootTable();

    void fromJson(QJsonObject &&root);
    QJsonObject toJson() const;

protected:
    void changeEvent(QEvent *e);

private:
    const QStringList types = {
        QStringLiteral("empty"),                QStringLiteral("entity"),
        QStringLiteral("block"),                QStringLiteral("chest"),
        QStringLiteral("fishing"),              QStringLiteral("gift"),
        QStringLiteral("advancement_reward"),   QStringLiteral("generic"),
        QStringLiteral("barter"),               QStringLiteral("command"),
        QStringLiteral("selector"),             QStringLiteral(
            "advancement_entity"),              QStringLiteral("archaelogy"),
        QStringLiteral("advancement_location"),
    };
    Ui::LootTable *ui;

    void updatePoolsTab(int size);
    void updateFunctionsTab(int size);
};

#endif // LOOTTABLE_H
