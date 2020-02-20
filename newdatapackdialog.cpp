#include "newdatapackdialog.h"
#include "ui_newdatapackdialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QDialogButtonBox>

NewDatapackDialog::NewDatapackDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewDatapackDialog)
{
    ui->setupUi(this);
    ui->locationInput->setText(QDir::currentPath());

    connect(ui->browseBtn, &QAbstractButton::clicked, this, &NewDatapackDialog::browse);

    connect(ui->formatInput, QOverload<int>::of(&QSpinBox::valueChanged), this, &NewDatapackDialog::checkOK);
    connect(ui->nameInput, &QLineEdit::textChanged, this, &NewDatapackDialog::checkOK);
    connect(ui->descInput, &QLineEdit::textChanged, this, &NewDatapackDialog::checkOK);
    connect(ui->locationInput, &QLineEdit::textChanged, this, &NewDatapackDialog::checkOK);

    checkOK();
}

NewDatapackDialog::~NewDatapackDialog()
{
    delete ui;
}

void NewDatapackDialog::browse() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open folder"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty())
        ui->locationInput->setText(dir);
}

void NewDatapackDialog::checkOK() {
    if(getName().isEmpty() || getDirPath().isEmpty())
        ui->dialogBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    else
        ui->dialogBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

QString NewDatapackDialog::getDesc() {
    return ui->descInput->text();
}

QString NewDatapackDialog::getName() {
    return ui->nameInput->text();
}

int NewDatapackDialog::getFormat() {
    return ui->formatInput->value();
}

QString NewDatapackDialog::getDirPath() {
    return ui->locationInput->text();
}
