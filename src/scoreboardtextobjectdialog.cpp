#include "scoreboardtextobjectdialog.h"
#include "ui_scoreboardtextobjectdialog.h"

#include "rawjsontextobjectinterface.h"

#include <QPushButton>

ScoreboardTextObjectDialog::ScoreboardTextObjectDialog(QWidget *parent) :
    AbstractTextObjectDialog(parent), ui(new Ui::ScoreboardTextObjectDialog) {
    ui->setupUi(this);

    connect(ui->nameEdit, &QLineEdit::textChanged, this,
            &ScoreboardTextObjectDialog::updateOkButton);
    connect(ui->objectiveEdit, &QLineEdit::textChanged, this,
            &ScoreboardTextObjectDialog::updateOkButton);
}

ScoreboardTextObjectDialog::~ScoreboardTextObjectDialog() {
    delete ui;
}

void ScoreboardTextObjectDialog::fromTextFormat(const QTextFormat &format) {
    m_format = format;

    ui->nameEdit->setText(format.stringProperty(ScoreboardName));
    ui->objectiveEdit->setText(format.stringProperty(ScoreboardObjective));
    ui->valueEdit->setText(format.stringProperty(ScoreboardValue));

    updateOkButton();
}

QTextFormat ScoreboardTextObjectDialog::toTextFormat() const {
    QTextFormat format = m_format;

    format.setProperty(ScoreboardName, ui->nameEdit->text());
    format.setProperty(ScoreboardObjective, ui->objectiveEdit->text());
    if (const auto &&value = ui->valueEdit->text(); !value.isEmpty()) {
        format.setProperty(ScoreboardValue, value);
    } else {
        format.clearProperty(ScoreboardValue);
    }

    return format;
}

void ScoreboardTextObjectDialog::changeEvent(QEvent *e) {
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void ScoreboardTextObjectDialog::updateOkButton() {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(
        ui->nameEdit->text().isEmpty() || ui->objectiveEdit->text().isEmpty());
}
