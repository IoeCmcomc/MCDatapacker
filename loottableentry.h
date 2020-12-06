#ifndef LOOTTABLEENTRY_H
#define LOOTTABLEENTRY_H

#include <QTabWidget>
#include <QHBoxLayout>

namespace Ui {
    class LootTableEntry;
}

class LootTableEntry : public QTabWidget
{
    Q_OBJECT

public:
    explicit LootTableEntry(QWidget *parent = nullptr);
    ~LootTableEntry();

    void fromJson(const QJsonObject &root);
    QJsonObject toJson() const;

    void resetAll();

    void setIsLuckBased(bool value);

    void setLootTableEditor(QWidget *value);

private slots:
    void onTypeChanged(int index);
    void onAddCondition();
    void onAddEntry();
    void onAddFunction();

private:
    Ui::LootTableEntry *ui;
    QWidget *lootTableEditor = nullptr;
    QHBoxLayout conditionsLayout;
    QHBoxLayout functionsLayout;
    QHBoxLayout entriesLayout;
    const QStringList entryTypes =
    { "empty", "item", "loot_table", "tag", "dynamic" };

    const int ENTRIES_TAB = 3;

    void reset(int index);
};

#endif /* LOOTTABLEENTRY_H */
