#include "translatedtextobjectdialog.h"
#include "ui_translatedtextobjectdialog.h"

#include "rawjsontextobjectinterface.h"
#include "rawjsontexteditor.h"

#include "game.h"

#include <QPushButton>

TranslatedTextObjectDialog::TranslatedTextObjectDialog(QWidget *parent)
    : AbstractTextObjectDialog(parent), ui(new Ui::TranslatedTextObjectDialog) {
    ui->setupUi(this);

    auto *editor = new RawJsonTextEditor(this);
    ui->widgetInterface->setupMainWidget(editor);
    ui->widgetInterface->mapToSetter(editor, &RawJsonTextEditor::fromJson);
    ui->widgetInterface->setFrameShape(QFrame::NoFrame);

    connect(ui->translateIdEdit, &QLineEdit::textChanged,
            this, &TranslatedTextObjectDialog::onTranslateIdChanged);
}

TranslatedTextObjectDialog::~TranslatedTextObjectDialog() {
    delete ui;
}

void TranslatedTextObjectDialog::fromTextFormat(const QTextFormat &format) {
    m_format = format;

    ui->translateIdEdit->setText(format.stringProperty(TranslateKey));
    ui->fallbackEdit->setText(format.stringProperty(TranslateFallback));
    ui->widgetInterface->setJson(format.property(TranslateArgs).toJsonArray());

    onTranslateIdChanged(ui->translateIdEdit->text());
}

QTextFormat TranslatedTextObjectDialog::toTextFormat() const {
    QTextFormat format = m_format;

    format.setProperty(TranslateKey, ui->translateIdEdit->text());
    format.setProperty(TranslateFallback, ui->fallbackEdit->text());
    format.setProperty(TranslateArgs, ui->widgetInterface->json());

    return format;
}

void TranslatedTextObjectDialog::changeEvent(QEvent *e) {
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void TranslatedTextObjectDialog::onTranslateIdChanged(const QString &text) {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(text.isEmpty());
}
