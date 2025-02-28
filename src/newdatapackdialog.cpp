#include "newdatapackdialog.h"
#include "ui_newdatapackdialog.h"

#include "game.h"

#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QDialogButtonBox>

struct PackFormatVersion {
    int     number = 1;
    QString versionRange;
};

const static QVector<PackFormatVersion> formatReleases = {
    {  4, QStringLiteral("1.13 – 1.14.4")   },
    {  5, QStringLiteral("1.15 – 1.16.1")   },
    {  6, QStringLiteral("1.16.2 – 1.16.5") },
    {  7, QStringLiteral("1.17 – 1.17.1")   },
    {  8, QStringLiteral("1.18 – 1.18.1")   },
    {  9, QStringLiteral("1.18.2")          },
    { 10, QStringLiteral("1.19 – 1.19.3")   },
    { 12, QStringLiteral("1.19.4")          },
    { 15, QStringLiteral("1.20 – 1.20.1")   },
    { 18, QStringLiteral("1.20.2")          },
    { 26, QStringLiteral("1.20.3 – 1.20.4") },
    { 41, QStringLiteral("1.20.5 – 1.20.6") },
    { 48, QStringLiteral("1.21 – 1.21.1")   },
    { 57, QStringLiteral("1.21.2 – 1.21.3") },
    { 61, QStringLiteral("1.21.4")          },
};

int gameVerToFormatReleasesIndex(const QVersionNumber &version) {
    if (version < Game::v1_15) {
        return 0;
    } else if (version < QVersionNumber(1, 16, 2)) {
        return 1;
    } else if (version < Game::v1_17) {
        return 2;
    } else if (version < Game::v1_18) {
        return 3;
    } else if (version < Game::v1_18_2) {
        return 4;
    } else if (version < Game::v1_19) {
        return 5;
    } else if (version < Game::v1_19_4) {
        return 6;
    } else if (version < Game::v1_20) {
        return 7;
    } else if (version < Game::v1_20_2) {
        return 8;
    } else if (version < QVersionNumber(1, 20, 3)) {
        return 9;
    } else if (version < QVersionNumber(1, 20, 5)) {
        return 10;
    } else if (version < Game::v1_21) {
        return 11;
    } else if (version < QVersionNumber(1, 21, 2)) {
        return 12;
    } else if (version < Game::v1_21_4) {
        return 13;
    } else {
        return 14;
    }
}

NewDatapackDialog::NewDatapackDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewDatapackDialog) {
    ui->setupUi(this);
    ui->locationInput->setText(QDir::currentPath());

    for (const auto &packFormat: formatReleases) {
        ui->gameVerCombo->addItem(packFormat.versionRange);
    }
    ui->gameVerCombo->setCurrentIndex(-1);

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
    connect(ui->gameVerCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &NewDatapackDialog::onGameVerComboChanged);

    createButton = new QPushButton(tr("Create"), this);
    ui->dialogBox->addButton(createButton, QDialogButtonBox::ActionRole);
    connect(createButton, &QAbstractButton::clicked, this, &QDialog::accept);

    ui->gameVerCombo->setCurrentIndex(gameVerToFormatReleasesIndex(
                                          Game::version()));

    checkOK();
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

void NewDatapackDialog::onGameVerComboChanged(const int index) {
    Q_ASSERT(index >= 0);
    ui->formatInput->setValue(formatReleases[index].number);
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
