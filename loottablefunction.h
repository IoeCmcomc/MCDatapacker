#ifndef LOOTTABLEFUNCTION_H
#define LOOTTABLEFUNCTION_H

#include <QTabWidget>

namespace Ui {
class LootTableFunction;
}

class LootTableFunction : public QTabWidget
{
    Q_OBJECT

public:
    explicit LootTableFunction(QWidget *parent = nullptr);
    ~LootTableFunction();

private:
    Ui::LootTableFunction *ui;
};

#endif // LOOTTABLEFUNCTION_H
