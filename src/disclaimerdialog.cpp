#include "disclaimerdialog.h"
#include "ui_disclaimerdialog.h"
#include "platforms/windows.h"

DisclaimerDialog::DisclaimerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisclaimerDialog) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    Windows::setDarkFrameIfDarkMode(this);
}

DisclaimerDialog::~DisclaimerDialog() {
    delete ui;
}
