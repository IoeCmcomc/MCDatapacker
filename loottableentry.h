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

private slots:
    void onTypeChanged(int index);
    void onAddCondition();
    void onAddEntry();

private:
    Ui::LootTableEntry *ui;
    QHBoxLayout conditionsLayout;
    QHBoxLayout entriesLayout;

    const int ENTRIES_TAB = 3;
};

#endif /* LOOTTABLEENTRY_H */
