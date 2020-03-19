#include "tagselectordialog.h"
#include "ui_tagselectordialog.h"

#include "mcrinvitem.h"
#include "mainwindow.h"

#include <QDebug>
#include <QElapsedTimer>

TagSelectorDialog::TagSelectorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TagSelectorDialog) {
    ui->setupUi(this);

    QElapsedTimer timer;
    timer.start();

    filterModel.setParent(ui->tagListView);
    model.setParent(this);
    setupTagTreeView();
    connect(ui->tagFilterBox, &QLineEdit::textChanged,
            [ = ](const QString &input) {
        filterModel.setFilterRegularExpression(input);
    });

    connect(ui->tagListView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &TagSelectorDialog::checkOK);

    connect(ui->tagListView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &TagSelectorDialog::showDetails);

    selectButton = new QPushButton(tr("Select"), this);
    ui->buttonBox->removeButton(ui->buttonBox->button(QDialogButtonBox::Ok));
    ui->buttonBox->addButton(selectButton, QDialogButtonBox::ActionRole);
    connect(selectButton,
            &QPushButton::clicked,
            this,
            &TagSelectorDialog::accept);

    checkOK();
    qDebug() << timer.elapsed();
}

TagSelectorDialog::~TagSelectorDialog() {
    delete ui;
}

void TagSelectorDialog::setupTagTreeView() {
    model.setParent(ui->tagListView);
    filterModel.setSourceModel(&model);
    filterModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    filterModel.setFilterRole(Qt::DisplayRole);
    ui->tagListView->setModel(&filterModel);

    /*auto MCRTagInfo = MainWindow::getMCRInfo("blockTag"); */
    MCRTagInfo = MainWindow::readMCRInfo(QStringLiteral("tags/items"));

    QMap<QString,
         QVariant>::const_iterator tagIter = MCRTagInfo.constBegin();
    int                            c       = 0;
    while ((tagIter != MCRTagInfo.constEnd())) {
        QStandardItem *item = new QStandardItem();
        item->setText(tagIter.key());
        model.appendRow(item);
        ++c;
        ++tagIter;
    }
}

QString TagSelectorDialog::getInternalSelectedID() {
    auto indexes = ui->tagListView->selectionModel()->selectedIndexes();

    /*    ui->tagListView->setParent(this); */

    if (indexes.isEmpty()) return "";

    QStandardItem *item =
        model.itemFromIndex(filterModel.mapToSource(indexes[0]));
    auto id = item->text();
    return id;
}

QString TagSelectorDialog::getSelectedID() {
    auto internalID = getInternalSelectedID();
    auto id         = internalID;

/*
      if (!internalID.contains(':'))
          id = "minecraft:" + id;
 */
    return QStringLiteral("#") + id;
}

void TagSelectorDialog::checkOK() {
    if (getInternalSelectedID().isEmpty())
        selectButton->setEnabled(false);
    else
        selectButton->setEnabled(true);
}

void TagSelectorDialog::showDetails() {
    auto id = getInternalSelectedID();

    if (!id.isEmpty()) {
        if (MCRTagInfo.contains(id)) {
            auto details = MCRTagInfo[id].toString();
            ui->tagDetailsLabel->setText(details);
            return;
        }
    }
    ui->tagDetailsLabel->setText(QString());
}