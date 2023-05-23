#include "entitynamestextobjectdialog.h"
#include "ui_entitynamestextobjectdialog.h"

#include "rawjsontextobjectinterface.h"

#include <QPushButton>

EntityNamesTextObjectDialog::EntityNamesTextObjectDialog(QWidget *parent) :
    AbstractTextObjectDialog(parent), ui(new Ui::EntityNamesTextObjectDialog) {
    ui->setupUi(this);

    connect(ui->selectorEdit, &QLineEdit::textChanged, this,
            &EntityNamesTextObjectDialog::onSelectorEditChanged);
}

EntityNamesTextObjectDialog::~EntityNamesTextObjectDialog() {
    delete ui;
}

void EntityNamesTextObjectDialog::fromTextFormat(const QTextFormat &format) {
    m_format = format;

    ui->selectorEdit->setText(format.stringProperty(Selector));
    ui->rawJsonEditor->fromJson(format.property(Separator).toJsonValue());
}

QTextFormat EntityNamesTextObjectDialog::toTextFormat() const {
    QTextFormat format = m_format;

    format.setProperty(Selector, ui->selectorEdit->text());
    if (!ui->rawJsonEditor->isEmpty()) {
        format.setProperty(Separator, ui->rawJsonEditor->toJson());
    } else {
        format.clearProperty(RawJsonProperty::Separator);
    }

    return format;
}

void EntityNamesTextObjectDialog::changeEvent(QEvent *e) {
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void EntityNamesTextObjectDialog::onSelectorEditChanged(const QString &text) {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(text.isEmpty());
}
