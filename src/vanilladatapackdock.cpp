#include "vanilladatapackdock.h"
#include "ui_vanilladatapackdock.h"

#include "globalhelpers.h"
#include "game.h"

#include <QMenu>
#include <QClipboard>

VanillaDatapackDock::VanillaDatapackDock(QWidget *parent)
    : QDockWidget(parent), ui(new Ui::VanillaDatapackDock) {
    ui->setupUi(this);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::doubleClicked, this,
            &VanillaDatapackDock::onDoubleClicked);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this,
            &VanillaDatapackDock::onCustomContextMenu);
    ui->treeView->setModel(&m_model);
}

VanillaDatapackDock::~VanillaDatapackDock() {
    delete ui;
}

void VanillaDatapackDock::changeEvent(QEvent *e) {
    QDockWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void VanillaDatapackDock::onDoubleClicked(const QModelIndex &index) {
    if (!index.isValid() || !m_model.isFile(index))
        return;

    auto catIndex    = index;
    auto subCatIndex = catIndex;
    while (catIndex.parent().isValid()) {
        subCatIndex = catIndex;
        catIndex    = catIndex.parent();
    }

    QString &&category = catIndex.data().toString();
    if (category == "tags"_QL1 || category == "worldgen"_QL1) {
        category += '/';
        category += subCatIndex.data().toString();
    }

    const QString &path    = m_model.filePath(index);
    QString        relPath = path;
    Glhp::removeInternalPrefix(relPath);
    const QString &&prefix = QStringLiteral("data-json/data/minecraft/") +
                             category + '/';
    Glhp::removePrefix(relPath, prefix);
    emit openFileRequested(path, Game::realVanillaFilePath(category, relPath));
}

void VanillaDatapackDock::onCustomContextMenu(const QPoint &pos) {
    const auto &index = ui->treeView->indexAt(pos);

    if (!index.isValid() || !m_model.isFile(index))
        return;

    QMenu    menu;
    QAction *copyIdAction = menu.addAction(
        tr("Copy resouce location"));

    connect(copyIdAction, &QAction::triggered, copyIdAction, [ = ](){
        QString &&relPath = m_model.filePath(index);
        Glhp::removeInternalPrefix(relPath);
        qApp->clipboard()->setText(
            Glhp::toNamespacedID(QStringLiteral("data-json"), relPath));
    });

    menu.exec(mapToGlobal(pos));
}
