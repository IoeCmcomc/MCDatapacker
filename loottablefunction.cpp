#include "loottablefunction.h"
#include "ui_loottablefunction.h"

LootTableFunction::LootTableFunction(QWidget *parent) :
      QTabWidget(parent),
      ui(new Ui::LootTableFunction)
{
    ui->setupUi(this);
}

LootTableFunction::~LootTableFunction()
{
    delete ui;
}
