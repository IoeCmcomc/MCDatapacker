#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->mainInfoLabel->setText(ui->mainInfoLabel->text().arg(qApp->
                                                             applicationVersion()));
}

AboutDialog::~AboutDialog() {
    delete ui;
}
