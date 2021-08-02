#include "tagselectordialog.h"
#include "ui_tagselectordialog.h"

#include "mcrinvitem.h"
#include "globalhelpers.h"
#include "mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

TagSelectorDialog::TagSelectorDialog(QWidget *parent,
                                     CodeFile::FileType type) :
    QDialog(parent),
    ui(new Ui::TagSelectorDialog) {
    ui->setupUi(this);

    filterModel.setParent(ui->tagListView);
    model.setParent(this);
    setupTagTreeView(type);
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
}

TagSelectorDialog::~TagSelectorDialog() {
    delete ui;
}

void TagSelectorDialog::setupTagTreeView(
    CodeFile::FileType type = CodeFile::ItemTag) {
    model.setParent(ui->tagListView);
    filterModel.setSourceModel(&model);
    filterModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    filterModel.setFilterRole(Qt::DisplayRole);
    ui->tagListView->setModel(&filterModel);

    QString tagStr = QStringLiteral("tag/");
    switch (type) {
    case CodeFile::BlockTag:
        tagStr += "block";
        break;

    case CodeFile::EntityTypeTag:
        tagStr += "entity_type";
        break;

    case CodeFile::FluidTag:
        tagStr += "fluid";
        break;

    case CodeFile::FunctionTag:
        tagStr += "function";
        break;

    case CodeFile::ItemTag:
        tagStr += "item";
        break;

    default:
        break;
    }

    auto tagStrSplited = tagStr.split('/');
    for (auto &tagStrSub : tagStrSplited)
        tagStrSub += 's';
    auto tagDir = tagStrSplited.join('/');

    auto fileIDList = Glhp::fileIdList(QDir::currentPath(), tagDir);
    for (const auto &id : fileIDList) {
        model.appendRow(new QStandardItem(id));
    }

    MCRTagInfo = MainWindow::readMCRInfo(tagStr);

    QMap<QString,
         QVariant>::const_iterator tagIter = MCRTagInfo.constBegin();
    while ((tagIter != MCRTagInfo.constEnd())) {
        auto *item = new QStandardItem(
            QStringLiteral("minecraft:") + tagIter.key());
        model.appendRow(item);
        ++tagIter;
    }
}

QString TagSelectorDialog::getInternalSelectedID() {
    auto indexes = ui->tagListView->selectionModel()->selectedIndexes();

    if (indexes.isEmpty()) return "";

    QStandardItem *item =
        model.itemFromIndex(filterModel.mapToSource(indexes[0]));
    auto id = item->text();
    return id;
}

QString TagSelectorDialog::getSelectedID() {
    auto internalID = getInternalSelectedID();
    auto id         = internalID;

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
        Glhp::removePrefix(id, "minecraft:");
        if (MCRTagInfo.contains(id)) {
            auto details = MCRTagInfo[id].toString();
            ui->tagDetailsLabel->setText(details);
            return;
        }
    }
    ui->tagDetailsLabel->setText(QString());
}
