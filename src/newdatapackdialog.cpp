#include "newdatapackdialog.h"
#include "ui_newdatapackdialog.h"

#include "game.h"

#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QDialogButtonBox>

NewDatapackDialog::NewDatapackDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewDatapackDialog) {
    ui->setupUi(this);
    ui->locationInput->setText(QDir::currentPath());

    const auto &&gameVer = Game::version();

    if (gameVer >= Game::v1_20_4)
        ui->formatInput->setValue(26);
    else if (gameVer >= Game::v1_20_2)
        ui->formatInput->setValue(18);
    else if (gameVer >= Game::v1_20)
        ui->formatInput->setValue(15);
    else if (gameVer >= Game::v1_19_4)
        ui->formatInput->setValue(12);
    else if (gameVer >= Game::v1_19)
        ui->formatInput->setValue(10);
    else if (gameVer >= Game::v1_18_2)
        ui->formatInput->setValue(9);
    else if (gameVer >= Game::v1_18)
        ui->formatInput->setValue(8);
    else if (gameVer >= Game::v1_17)
        ui->formatInput->setValue(7);
    else if (gameVer >= QVersionNumber(1, 16, 2))
        ui->formatInput->setValue(6);
    else if (gameVer >= Game::v1_15)
        ui->formatInput->setValue(5);
    else if (gameVer >= QVersionNumber(1, 13))
        ui->formatInput->setValue(4);

    connect(ui->browseBtn, &QAbstractButton::clicked,
            this, &NewDatapackDialog::browse);

    connect(ui->formatInput, qOverload<int>(&QSpinBox::valueChanged),
            this, &NewDatapackDialog::checkOK);
    connect(ui->nameInput, &QLineEdit::textChanged,
            this, &NewDatapackDialog::checkOK);
    connect(ui->descInput, &QPlainTextEdit::textChanged,
            this, &NewDatapackDialog::checkOK);
    connect(ui->locationInput, &QLineEdit::textChanged,
            this, &NewDatapackDialog::checkOK);
    connect(ui->formatInput, qOverload<int>(&QSpinBox::valueChanged),
            this, &NewDatapackDialog::onFormatSpinChanged);

    createButton = new QPushButton(tr("Create"), this);
    ui->dialogBox->addButton(createButton, QDialogButtonBox::ActionRole);
    connect(createButton, &QAbstractButton::clicked, this, &QDialog::accept);

    checkOK();
    onFormatSpinChanged(ui->formatInput->value());
}

NewDatapackDialog::~NewDatapackDialog() {
    delete ui;
}

void NewDatapackDialog::browse() {
    QString &&dir =
        QFileDialog::getExistingDirectory(
            this, tr("Choose folder to create new datapack"),
            QString(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
        ui->locationInput->setText(dir);
}

void NewDatapackDialog::checkOK() {
    createButton->setDisabled(getName().isEmpty() || getDirPath().isEmpty());
}

void NewDatapackDialog::onFormatSpinChanged(const int format) {
    const static QMap<int, QString> formatReleases = {
        {  4, QStringLiteral("1.13–1.14.4")   },
        {  5, QStringLiteral("1.15–1.16.1")   },
        {  6, QStringLiteral("1.16.2–1.16.5") },
        {  7, QStringLiteral("1.17–1.17.1")   },
        {  8, QStringLiteral("1.18–1.18.1")   },
        {  9, QStringLiteral("1.18.2")        },
        { 10, QStringLiteral("1.19–1.19.3")   },
        { 12, QStringLiteral("1.19.4")        },
        { 15, QStringLiteral("1.20–1.20.1")   },
        { 18, QStringLiteral("1.20.2")        },
        { 26, QStringLiteral("1.20.3–1.20.4") },
    };

    Q_ASSERT(format >= 0);

    ui->formatDisplay->setText(formatReleases.value(format, {}));
}

QString NewDatapackDialog::getDesc() {
    return ui->descInput->toPlainText();
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
