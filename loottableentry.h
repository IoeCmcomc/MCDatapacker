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

    void setIsLuckBased(bool value);

    void setLootTableEditor(QWidget *value);

private slots:
    void onTypeChanged(int index);
    void onLuckBasedChanged(bool checked);
    void onAddCondition();
    void onAddEntry();

private:
    Ui::LootTableEntry *ui;
    bool isLuckBased         = false;
    QWidget *lootTableEditor = nullptr;
    QHBoxLayout conditionsLayout;
    QHBoxLayout entriesLayout;
    const QStringList entryTypes =
    { "empty", "item", "loot_table", "tag", "group", "sequence" };

    const int ENTRIES_TAB = 3;
};

#endif /* LOOTTABLEENTRY_H */
