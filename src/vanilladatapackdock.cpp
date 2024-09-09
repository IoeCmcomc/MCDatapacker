#include "vanilladatapackdock.h"
#include "ui_vanilladatapackdock.h"

#include "tabbeddocumentinterface.h"

#include "globalhelpers.h"
#include "game.h"

#include <QMenu>
#include <QClipboard>
#include <QSaveFile>
#include <QMessageBox>

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

void VanillaDatapackDock::setPackOpened(const bool value) {
    m_packOpened = value;
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

    const QString  &path       = m_model.filePath(index);
    const QString &&actualPath = realPath(index);

    emit openFileRequested(path, actualPath);
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

    if (m_packOpened) {
        QMenu *duplicateMenu = menu.addMenu(tr("Duplicate to"));

        QDir dataDir(QDir::current());
        Q_UNUSED(dataDir.cd(QStringLiteral("data")));
        const auto &&nspaceDirs = dataDir.entryList(
            QDir::Dirs | QDir::NoDotAndDotDot);
        for (const auto &nspaceDir : nspaceDirs) {
            QAction *duplicateAction = duplicateMenu->addAction(nspaceDir);
            connect(duplicateAction, &QAction::triggered, duplicateAction,
                    [ = ](){
                duplicateFile(index, nspaceDir);
            });
        }
    }

    menu.exec(mapToGlobal(pos));
}

QString VanillaDatapackDock::realPath(const QModelIndex &index) const {
    if (!index.isValid() || !m_model.isFile(index))
        return {};

    auto catIndex    = index;
    auto subCatIndex = catIndex;
    while (catIndex.parent().isValid()) {
        subCatIndex = catIndex;

        catIndex = catIndex.parent();
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
    return Game::realVanillaFilePath(category, relPath);
}

void VanillaDatapackDock::duplicateFile(const QModelIndex &index,
                                        const QString &nspace) {
    if (!index.isValid() || !m_model.isFile(index))
        return;

    const QString &&vanillaPath = ":/minecraft/"_QL1 +
                                  Game::versionString() +
                                  "/data-json/data/minecraft"_QL1;
    const QString &&vanillaFilePath = m_model.filePath(index);

    const QString &&dirPath  = QDir::currentPath();
    const QString &&filePath = dirPath + "/data/"_QL1 + nspace + '/' +
                               Glhp::relPath(vanillaPath, vanillaFilePath);
    const QString &&fileDirPath = QFileInfo(filePath).path();
    if (!QFileInfo::exists(filePath))
        QDir().mkpath(fileDirPath);

    QSaveFile newFile(filePath);
    QString   errorMessage;
    if (newFile.open(QIODevice::WriteOnly)) {
        QTextStream out(&newFile);
        out.setCodec("UTF-8");
        bool ok = false;

        const QString &&actualPath = realPath(index);
        out << TabbedDocumentInterface::readTextFile(
            this, actualPath, ok);

        if (!newFile.commit()) {
            errorMessage = tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(filePath),
                                newFile.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                       .arg(QDir::toNativeSeparators(filePath),
                            newFile.errorString());
    }
    if (!errorMessage.isEmpty()) {
        QMessageBox::information(this, tr("Error"), errorMessage);
    } else {
        emit openFileRequested(filePath);
    }
}
