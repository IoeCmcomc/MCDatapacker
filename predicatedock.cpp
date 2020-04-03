#include "predicatedock.h"
#include "ui_predicatedock.h"

PredicateDock::PredicateDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PredicateDock) {
    ui->setupUi(this);

    ui->condition->setIsModular(false);
}

PredicateDock::~PredicateDock() {
    delete ui;
}
