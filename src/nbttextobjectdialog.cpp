#include "nbttextobjectdialog.h"
#include "ui_nbttextobjectdialog.h"

#include "rawjsontextobjectinterface.h"

#include <QPushButton>

NbtTextObjectDialog::NbtTextObjectDialog(QWidget *parent) :
    AbstractTextObjectDialog(parent), ui(new Ui::NbtTextObjectDialog) {
    ui->setupUi(this);

    connect(ui->nbtPathEdit, &QLineEdit::textChanged,
            this, &NbtTextObjectDialog::updateOkButton);
    connect(ui->blockEdit, &QLineEdit::textChanged,
            this, &NbtTextObjectDialog::updateOkButton);
    connect(ui->entityEdit, &QLineEdit::textChanged,
            this, &NbtTextObjectDialog::updateOkButton);
    connect(ui->storageEdit, &QLineEdit::textChanged,
            this, &NbtTextObjectDialog::updateOkButton);
}

NbtTextObjectDialog::~NbtTextObjectDialog() {
    delete ui;
}

void NbtTextObjectDialog::fromTextFormat(const QTextFormat &format) {
    m_format = format;

    ui->nbtPathEdit->setText(format.stringProperty(NbtPath));
    if (format.hasProperty(NbtInterpret)) {
        ui->interpretCheck->setChecked(format.boolProperty(NbtInterpret));
    }
    ui->blockEdit->setText(format.stringProperty(NbtBlock));
    ui->entityEdit->setText(format.stringProperty(NbtEntity));
    ui->storageEdit->setText(format.stringProperty(NbtStorage));
    ui->rawJsonEditor->fromJson(format.property(Separator).toJsonValue());

    updateOkButton();
}

QTextFormat NbtTextObjectDialog::toTextFormat() const {
    QTextFormat format = m_format;

    format.setProperty(NbtPath, ui->nbtPathEdit->text());
    switch (ui->interpretCheck->checkState()) {
        case Qt::Checked: {
            format.setProperty(RawJsonProperty::NbtInterpret, true);
            break;
        }
        case Qt::Unchecked: {
            format.setProperty(RawJsonProperty::NbtInterpret, false);
        }
        case Qt::PartiallyChecked: {
            format.clearProperty(RawJsonProperty::NbtInterpret);
            break;
        }
    }
    format.setProperty(RawJsonProperty::NbtBlock, ui->blockEdit->text());
    format.setProperty(RawJsonProperty::NbtEntity, ui->entityEdit->text());
    format.setProperty(RawJsonProperty::NbtStorage, ui->storageEdit->text());
    if (!ui->rawJsonEditor->isEmpty()) {
        format.setProperty(Separator, ui->rawJsonEditor->toJson());
    } else {
        format.clearProperty(RawJsonProperty::Separator);
    }

    return format;
}

void NbtTextObjectDialog::changeEvent(QEvent *e) {
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void NbtTextObjectDialog::updateOkButton() {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(
        ui->nbtPathEdit->text().isEmpty()
        || (ui->blockEdit->text().isEmpty() &&
            ui->entityEdit->text().isEmpty() &&
            ui->storageEdit->text().isEmpty()));
}
