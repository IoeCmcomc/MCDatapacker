#include "disclaimerdialog.h"
#include "ui_disclaimerdialog.h"

DisclaimerDialog::DisclaimerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisclaimerDialog) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
}

DisclaimerDialog::~DisclaimerDialog() {
    delete ui;
}
