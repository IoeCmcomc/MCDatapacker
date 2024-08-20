#include "idtagselectordialog.h"
#include "ui_idtagselectordialog.h"

#include <QJsonArray>
#include <QPushButton>

IdTagSelectorDialog::IdTagSelectorDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::IdTagSelectorDialog) {
    ui->setupUi(this);
    ui->tableWidget->appendColumnMapping({}, ui->comboBox);

    connect(ui->buttonBox->button(QDialogButtonBox::Reset),
            &QPushButton::clicked,
            ui->tableWidget, &ExtendedTableWidget::clear);
}

IdTagSelectorDialog::~IdTagSelectorDialog() {
    delete ui;
}

void IdTagSelectorDialog::setIdDisplayName(const QString &name) {
    ui->tableWidget->setColumnTitles({ name, });
    ui->label->setText(name + ':');
}

void IdTagSelectorDialog::setIdModel(QAbstractListModel *model) {
    ui->comboBox->setModel(model);
}

void IdTagSelectorDialog::fromJson(const QJsonArray &arr) {
    ui->tableWidget->fromJson(arr);
}

QJsonArray IdTagSelectorDialog::toJson() const {
    return ui->tableWidget->toJsonArray();
}

void IdTagSelectorDialog::changeEvent(QEvent *e) {
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}
