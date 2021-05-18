#ifndef LOOTTABLEPOOL_H
#define LOOTTABLEPOOL_H

#include <QTabWidget>

namespace Ui {
    class LootTablePool;
}

class LootTablePool : public QTabWidget
{
    Q_OBJECT

public:
    explicit LootTablePool(QWidget *parent = nullptr);
    ~LootTablePool();

    QJsonObject toJson() const;
    void fromJson(QJsonObject root);

protected:
    void changeEvent(QEvent *event) override;

private:
    Ui::LootTablePool *ui;

    void reset();
};

#endif /* LOOTTABLEPOOL_H */
