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

private /*slots*/ :
    void onTypeChanged(int index);
    void onTabChanged(int index);

private:
    Ui::LootTableEntry *ui;
    const QStringList entryTypes =
    { "empty", "item", "loot_table", "tag", "dynamic" };

    static constexpr const int ENTRIES_TAB = 1;

    void reset(int index);
    void initCondInterface();
    void initFuncInterface();
    void initEntryInterface();
};

#endif /* LOOTTABLEENTRY_H */
