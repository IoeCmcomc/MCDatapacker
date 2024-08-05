#include "numberproviderdialog.h"
#include "ui_numberproviderdialog.h"

#include "game.h"

NumberProviderDialog::NumberProviderDialog(QWidget *parent)
    : QDialog(parent), m_controller{nullptr, "type"}, ui(
        new Ui::NumberProviderDialog) {
    ui->setupUi(this);

    m_parent = qobject_cast<NumberProvider *>(
        parent->parent()->parent()->parent());
    Q_ASSERT(m_parent != nullptr);

    ui->exactSpin->setIntegerOnly(m_parent->isIntegerOnly());
    ui->minValueInput->setIntegerOnly(m_parent->isIntegerOnly());
    ui->maxValueInput->setIntegerOnly(m_parent->isIntegerOnly());

    if (Game::version() < Game::v1_17) {
        ui->minValueInput->setModes(
            ui->minValueInput->getModes() & (~NumberProvider::Advanced));
        ui->maxValueInput->setModes(
            ui->maxValueInput->getModes() & (~NumberProvider::Advanced));
        ui->numberInput->setModes(
            ui->numberInput->getModes() & (~NumberProvider::Advanced));
        ui->probInput->setModes(
            ui->probInput->getModes() & (~NumberProvider::Advanced));

        ui->multipagewidget->setPageHidden(ui->score,
                                           true);
    }
    ui->multipagewidget->setPageHidden(ui->storage,
                                       Game::version() < Game::v1_20_6);
    ui->multipagewidget->setPageHidden(ui->enchantLevel,
                                       Game::version() < Game::v1_21);

    m_controller.setWidget(ui->multipagewidget);
    m_controller.addChoices({
        { "constant", 0, new DWCRecord{ { "value", ui->exactSpin } } },
        { "uniform", 1, new DWCRecord{
              { "min", ui->minValueInput },
              { "max", ui->maxValueInput },
          } },
        { "binomial", 2, new DWCRecord{
              { "n", ui->numberInput },
              { "p", ui->probInput },
          }, },
        { "score", 3, new DWCRecord{
              { "score", ui->scoreEdit },
              { "scale", ui->scaleSpin },
              { "target", new DWCBtnGrp{
                    ui->buttonGroup, "type",
                    {
                        { "context", 0, new DWCRecord{
                              { "target", ui->contextCombo },
                          } },
                        { "fixed", 1, new DWCRecord{
                              { "name", ui->nameEdit },
                          } },
                    } } } } },
        { "storage", 4, new DWCRecord{
              { "storage", ui->storageEdit },
              { "path", ui->nbtPathEdit },
          } },
        { "enchantment_level", 5, new DWCRecord{ { "amount", ui->enchantLevelBtn } } }
    });
}

NumberProviderDialog::~NumberProviderDialog() {
    delete ui;
}

QJsonObject NumberProviderDialog::toJson() const {
    QJsonObject obj;

    m_controller.putValueTo(obj, {});
    return obj;
}

void NumberProviderDialog::fromJson(const QJsonObject &value) {
    m_controller.setValueFrom(value, {});
}

void NumberProviderDialog::changeEvent(QEvent *e) {
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}
