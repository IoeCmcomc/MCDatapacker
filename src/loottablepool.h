#ifndef LOOTTABLEPOOL_H
#define LOOTTABLEPOOL_H

#include "datawidgetcontroller.h"

#include <QTabWidget>

#include <mutex>


namespace Ui {
    class LootTablePool;
}

class LootTablePool : public QTabWidget {
    Q_OBJECT

public:
    explicit LootTablePool(QWidget *parent = nullptr);
    ~LootTablePool();

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &root);

protected:
    void changeEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;

private /*slots*/ :
    void updateEntriesTab(int size);
    void updateFunctionsTab(int size);
    void updateConditionsTab(int size);

private:
    DataWidgetControllerRecord m_controller;
    Ui::LootTablePool *ui;
    std::once_flag m_fullyInitialized;

    void reset();
    void init();
    void updateCounts();
};

#endif /* LOOTTABLEPOOL_H */
