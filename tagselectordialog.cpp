#include "tagselectordialog.h"
#include "ui_tagselectordialog.h"

#include "mcrinvitem.h"
#include "globalhelpers.h"
#include "mainwindow.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QDir>
#include <QFileInfo>

TagSelectorDialog::TagSelectorDialog(QWidget *parent,
                                     MainWindow::MCRFileType type) :
    QDialog(parent),
    ui(new Ui::TagSelectorDialog) {
    ui->setupUi(this);

    QElapsedTimer timer;
    timer.start();

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
    qDebug() << timer.elapsed();
}

TagSelectorDialog::~TagSelectorDialog() {
    delete ui;
}

void TagSelectorDialog::setupTagTreeView(
    MainWindow::MCRFileType type = MainWindow::ItemTag) {
    model.setParent(ui->tagListView);
    filterModel.setSourceModel(&model);
    filterModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    filterModel.setFilterRole(Qt::DisplayRole);
    ui->tagListView->setModel(&filterModel);

    /*auto MCRTagInfo = MainWindow::getMCRInfo("blockTag"); */

    QString tagStr = QStringLiteral("tags/");
    switch (type) {
    case MainWindow::BlockTag:
        tagStr += "blocks";
        break;

    case MainWindow::EntityTypeTag:
        tagStr += "entity_types";
        break;

    case MainWindow::FluidTag:
        tagStr += "fluids";
        break;

    case MainWindow::FunctionTag:
        tagStr += "functions";
        break;

    case MainWindow::ItemTag:
        tagStr += "items";
        break;

    default:
        break;
    }

    QString dataPath = MainWindow::getCurDir() + QStringLiteral("/data/");
    QDir    dir(dataPath);
    auto    nspaceDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (auto nspaceDir : nspaceDirs) {
        auto    nspace  = nspaceDir.section('.', 0, 0);
        QString tagPath = dataPath + nspace + '/' +
                          tagStr;
        QDir tagDir(tagPath);
        if (tagDir.exists() && (!tagDir.isEmpty())) {
            auto tagNames =
                tagDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
            for (auto tagName : tagNames) {
                tagName = tagName.section('.', 0, 0);

                QStandardItem *item = new QStandardItem();
                item->setText(nspace + ":" + tagName);
                model.appendRow(item);
            }
        }
    }

    MCRTagInfo = MainWindow::readMCRInfo(tagStr);

    QMap<QString,
         QVariant>::const_iterator tagIter = MCRTagInfo.constBegin();
    int                            c       = 0;
    while ((tagIter != MCRTagInfo.constEnd())) {
        QStandardItem *item = new QStandardItem();
        item->setText("minecraft:" + tagIter.key());
        model.appendRow(item);
        ++c;
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
        if (id.startsWith("minecraft:"))
            id.remove(0, 10);
        if (MCRTagInfo.contains(id)) {
            auto details = MCRTagInfo[id].toString();
            ui->tagDetailsLabel->setText(details);
            return;
        }
    }
    ui->tagDetailsLabel->setText(QString());
}
