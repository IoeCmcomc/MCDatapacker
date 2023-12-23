#ifndef LOOTTABLEPOOL_H
#define LOOTTABLEPOOL_H

#include <QTabWidget>

namespace Ui {
    class LootTablePool;
}

class LootTablePool : public QTabWidget {
    Q_OBJECT

public:
    explicit LootTablePool(QWidget *parent = nullptr);
    ~LootTablePool();

    QJsonObject toJson() const;
    void fromJson(QJsonObject root);

protected:
    void changeEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::LootTablePool *ui;

    void reset();
    void init();

    std::once_flag m_fullyInitialized;

private /*slots*/ :
    void updateEntriesTab(int size);
    void updateFunctionsTab(int size);
    void updateConditionsTab(int size);
};

#endif /* LOOTTABLEPOOL_H */
