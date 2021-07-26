#include "datapacktreeview.h"

#include "mainwindow.h"
#include "globalhelpers.h"

#include <QDebug>
#include <QModelIndex>
#include <QFile>
#include <QDropEvent>
#include <QMimeData>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QSaveFile>
#include <QToolTip>
#include <QApplication>
#include <QClipboard>

DatapackTreeView::DatapackTreeView(QWidget *parent) : QTreeView(parent) {
    dirModel.setReadOnly(false);
    dirModel.setIconProvider(&iconProvider);

/*
      connect(this, SIGNAL(doubleClicked(QModelIndex)), this,
              SLOT(onDoubleClicked(QModelIndex)));
 */
    connect(this, &QTreeView::doubleClicked,
            this, &DatapackTreeView::onDoubleClicked);
    connect(this, &QTreeView::expanded,
            this, &DatapackTreeView::resizeFirstColumn);
    connect(this, &QTreeView::collapsed,
            this, &DatapackTreeView::resizeFirstColumn);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &DatapackTreeView::customContextMenuRequested, this,
            &DatapackTreeView::onCustomContextMenu);
    connect(&dirModel, &QFileSystemModel::fileRenamed,
            this, &DatapackTreeView::onFileRenamed);
}

QMenu *DatapackTreeView::mkContextMenu(QModelIndex index) {
    /*Right click context menu */
    auto       *cMenu = new QMenu(this);
    QString     path  = Glhp::relPath(dirPath, dirModel.filePath(index));
    QFileInfo &&finfo = dirModel.fileInfo(index);

    if (finfo.isFile()) {
        QAction *cMenuActionOpen = new QAction(tr("Open"), cMenu);
        cMenuActionOpen->setShortcuts(QKeySequence::Open);
        connect(cMenuActionOpen, &QAction::triggered,
                this, &DatapackTreeView::contextMenuOnOpen);
        cMenu->addAction(cMenuActionOpen);
    }

    const auto &&fileType = Glhp::pathToFileType(dirPath, finfo.filePath());
    if ((fileType == CodeFile::Function || fileType >= CodeFile::JsonText) &&
        path.startsWith(QStringLiteral("data/"))) {
        const QString &&filePathId = Glhp::toNamespacedID(dirPath,
                                                          finfo.filePath());

        QAction *cMenuActionCopyId =
            new QAction(tr("Copy namespaced ID"), cMenu);
        if (filePathId.isEmpty())
            cMenuActionCopyId->setDisabled(true);
        connect(cMenuActionCopyId, &QAction::triggered,
                this, &DatapackTreeView::contextMenuOnCopyId);
        cMenu->addAction(cMenuActionCopyId);

        if (fileType == CodeFile::Function) {
            QAction *cMenuActionInLoadDotJson
                = new QAction(tr("Run this function when loaded"), cMenu);
            cMenuActionInLoadDotJson->setCheckable(true);
            cMenuActionInLoadDotJson->setChecked(isStringInTagFile(dirPath +
                                                                   "/data/minecraft/tags/functions/load.json",
                                                                   filePathId));
            connect(cMenuActionInLoadDotJson, &QAction::triggered, this,
                    [this, filePathId](bool checked) {
                this->contextMenuModifyTagFile
                    (dirPath +
                    QStringLiteral("/data/minecraft/tags/functions/load.json"),
                    filePathId, checked);
            });
            cMenu->addAction(cMenuActionInLoadDotJson);

            QAction *cMenuActionInTickDotJson
                = new QAction(tr("Run this function every tick"), cMenu);
            cMenuActionInTickDotJson->setCheckable(true);
            cMenuActionInTickDotJson->setChecked(
                isStringInTagFile(dirPath +
                                  QStringLiteral(
                                      "/data/minecraft/tags/functions/tick.json"),
                                  filePathId));
            connect(cMenuActionInTickDotJson, &QAction::triggered, this,
                    [this, filePathId](bool checked) {
                this->contextMenuModifyTagFile
                    (dirPath +
                    QStringLiteral("/data/minecraft/tags/functions/tick.json"),
                    filePathId, checked);
            });
            cMenu->addAction(cMenuActionInTickDotJson);
        }
    }


    if (path != QStringLiteral("data") &&
        path != QStringLiteral("pack.mcmeta")) {
        QAction *cMenuActionRename = new QAction(tr("Rename"), cMenu);
        connect(cMenuActionRename, &QAction::triggered,
                this, &DatapackTreeView::contextMenuOnRename);
        cMenu->addAction(cMenuActionRename);

        QAction *cMenuActionDelete = new QAction(tr("Delete"), cMenu);
        connect(cMenuActionDelete, &QAction::triggered,
                this, &DatapackTreeView::contextMenuOnDelete);
        cMenu->addAction(cMenuActionDelete);
    }

    if (path != QStringLiteral("pack.mcmeta")) {
        cMenu->addSeparator();
        QMenu *newMenu = new QMenu(tr("New"), cMenu); /*"New" menu */

        QAction *newMenuNewFolder = new QAction(tr("Folder"), newMenu);
        if (path == QStringLiteral("data"))
            newMenuNewFolder->setText(tr("Namespace"));
        connect(newMenuNewFolder, &QAction::triggered,
                this, &DatapackTreeView::contextMenuOnNewFolder);
        newMenu->addAction(newMenuNewFolder);

        if (path != QStringLiteral("data")) {
            newMenu->addSeparator();

            QAction *newMenuNewAdv = new QAction(tr("Advancement"), newMenu);
            connect(newMenuNewAdv, &QAction::triggered, [this]() {
                this->contextMenuOnNew
                (
                    "advancement_" + Glhp::randStr() + ".json",
                    QStringLiteral("advancements"));
            });
            newMenu->addAction(newMenuNewAdv);

            QAction *newMenuNewFunct = new QAction(tr("Function"), newMenu);
            connect(newMenuNewFunct, &QAction::triggered, [this]() {
                this->contextMenuOnNew
                (
                    "function_" + Glhp::randStr() + ".mcfunction",
                    QStringLiteral("functions"));
            });
            newMenu->addAction(newMenuNewFunct);

            QAction *newMenuNewLoot = new QAction(tr("Loot table"), newMenu);
            connect(newMenuNewLoot, &QAction::triggered, [this]() {
                this->contextMenuOnNew
                    ("loot_table_" + Glhp::randStr() + ".json",
                    QStringLiteral("loot_tables"));
            });
            newMenu->addAction(newMenuNewLoot);

            QAction *newMenuNewPred = new QAction(tr("Predicate"), newMenu);
            connect(newMenuNewPred, &QAction::triggered, [this]() {
                this->contextMenuOnNew
                    ("predicate_" + Glhp::randStr() + ".json",
                    QLatin1String("predicates"));
            });
            newMenu->addAction(newMenuNewPred);

            QAction *newMenuNewRecipe = new QAction(tr("Recipe"), newMenu);
            connect(newMenuNewRecipe, &QAction::triggered, [this]() {
                this->contextMenuOnNew
                    ("recipe_" + Glhp::randStr() + ".json",
                    QStringLiteral("recipes"));
            });
            newMenu->addAction(newMenuNewRecipe);

            QAction *newMenuNewStruct = new QAction(tr("Structure"), newMenu);
            newMenuNewStruct->setDisabled(true);
            connect(newMenuNewStruct, &QAction::triggered, [this]() {
                this->contextMenuOnNew
                    ("structure_" + Glhp::randStr() + ".nbt",
                    QStringLiteral("structures"));
            });
            newMenu->addAction(newMenuNewStruct);

            /*"Tag" menu */
            QMenu *tagMenu = new QMenu(tr("Tag"), newMenu);

            QAction *newBlockTag = new QAction(tr("Blocks"), tagMenu);
            connect(newBlockTag, &QAction::triggered, [this]() {
                this->contextMenuOnNew
                    ("tag_" + Glhp::randStr() + ".json",
                    QStringLiteral("tags/blocks"));
            });
            tagMenu->addAction(newBlockTag);

            QAction *newEntityTag = new QAction(tr("Entity types"), tagMenu);
            connect(newEntityTag, &QAction::triggered, [this]() {
                this->contextMenuOnNew
                    ("tag_" + Glhp::randStr() + ".json",
                    QStringLiteral("tags/entity_types"));
            });
            tagMenu->addAction(newEntityTag);

            QAction *newFluidTag = new QAction(tr("Fluids"), tagMenu);
            connect(newFluidTag, &QAction::triggered, [this]() {
                this->contextMenuOnNew
                    ("tag_" + Glhp::randStr() + ".json",
                    QStringLiteral("tags/fluids"));
            });
            tagMenu->addAction(newFluidTag);

            QAction *newFunctTag = new QAction(tr("Functions"), tagMenu);
            connect(newFunctTag, &QAction::triggered, [this]() {
                this->contextMenuOnNew
                    ("tag_" + Glhp::randStr() + ".json",
                    QStringLiteral("tags/functions"));
            });
            tagMenu->addAction(newFunctTag);

            QAction *newItemTag = new QAction(tr("Items"), tagMenu);
            connect(newItemTag, &QAction::triggered, [this]() {
                this->contextMenuOnNew
                    ("tag_" + Glhp::randStr() + ".json",
                    QStringLiteral("tags/items"));
            });
            tagMenu->addAction(newItemTag);

            newMenu->addMenu(tagMenu);
            newMenu->addSeparator();

            QAction *newMenuNewFile = new QAction(tr("File"), tagMenu);
            connect(newMenuNewFile, &QAction::triggered, [this]() {
                this->contextMenuOnNew("file" + Glhp::randStr() + ".txt");
            });
            newMenu->addAction(newMenuNewFile);
        }
        cMenu->addMenu(newMenu);
    }

    return cMenu;
}

QModelIndex DatapackTreeView::getSelected() {
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();

    return (!indexes.isEmpty()) ? indexes.at(0) : QModelIndex();
}

void DatapackTreeView::load(const QDir &dir) {
    dirPath = dir.path();
    dirModel.setRootPath(dirPath);
    setModel(&dirModel);

    for (int i = 1; i < dirModel.columnCount(); ++i)
        hideColumn(i);

    setRootIndex(dirModel.index(dirPath));
    resizeFirstColumn();

    emit datapackChanged();
}


bool DatapackTreeView::event(QEvent *event) {
    if (event->type() == QEvent::ToolTip) {
        auto helpEvent = static_cast<QHelpEvent*>(event);
        auto pos       = viewport()->mapFrom(this, helpEvent->pos());
        auto index     = indexAt(pos);

        if (visualRect(index).contains(pos)) {
            QToolTip::showText(helpEvent->globalPos(),
                               dirModel.filePath(index));
        } else {
            QToolTip::hideText();
        }
        return true;
    } else {
        return QTreeView::event(event);
    }
}


void DatapackTreeView::dragEnterEvent(QDragEnterEvent *event) {
    if (dirPath.isEmpty() || event->mimeData()->urls().isEmpty()) {
        QTreeView::dragEnterEvent(event);
        return;
    }
    QModelIndex draggedIndex = dirModel.index(
        event->mimeData()->urls().at(0).toLocalFile()
        );
    if (event->keyboardModifiers() & Qt::ShiftModifier
        || draggedIndex == dirModel.index(dirPath + "/data")
        || draggedIndex == dirModel.index(dirPath + "/pack.mcmeta")) {
        event->setDropAction(Qt::IgnoreAction);
        event->ignore();
    } else if (event->keyboardModifiers() & Qt::ControlModifier) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    QTreeView::dragEnterEvent(event);
}

void DatapackTreeView::onCustomContextMenu(const QPoint &point) {
    QModelIndex index = this->indexAt(point);

    if (index.isValid()) {
        QMenu *cMenu = mkContextMenu(index);
        cMenuPos = point;
        cMenu->exec(this->viewport()->mapToGlobal(point));
        delete cMenu;
    }
}

void DatapackTreeView::onFileRenamed(const QString &path,
                                     const QString &oldName,
                                     const QString &newName) {
/*    qDebug() << "DatapackTreeView::onFileRenamed" << oldName << newName; */
    QString oldpath = path + '/' + oldName;
    QString newpath = path + '/' + newName;

    if (Glhp::pathToFileType(dirPath, oldpath) == CodeFile::Function) {
        if (isStringInTagFile(dirPath +
                              "/data/minecraft/tags/functions/load.json",
                              Glhp::toNamespacedID(dirPath, oldpath))) {
            contextMenuModifyTagFile
                (dirPath + "/data/minecraft/tags/functions/load.json",
                Glhp::toNamespacedID(dirPath, oldpath), false);
            if (Glhp::pathToFileType(dirPath, newpath) == CodeFile::Function) {
                if (!isStringInTagFile(dirPath +
                                       "/data/minecraft/tags/functions/load.json",
                                       Glhp::toNamespacedID(dirPath,
                                                            newpath))) {
                    contextMenuModifyTagFile
                        (dirPath + "/data/minecraft/tags/functions/load.json",
                        Glhp::toNamespacedID(dirPath, newpath), true);
                }
            }
        }
    }

    emit fileRenamed(path, oldName, newName);
}

void DatapackTreeView::contextMenuOnNewFolder() {
    /*QModelIndex selected = getSelected(); */
    QModelIndex selected = indexAt(cMenuPos);

    if (selected.isValid()) {
        const QFileInfo finfo = dirModel.fileInfo(selected);
        QModelIndex     newDir;
        if (finfo.exists() && finfo.isFile())
            selected = selected.parent();
        newDir = dirModel.mkdir(selected, "new_folder_" + Glhp::randStr());
        setCurrentIndex(newDir);
        edit(newDir);
    }
}

QModelIndex DatapackTreeView::makeNewFile(QModelIndex index,
                                          const QString &name,
                                          const QString &catDir,
                                          const QString &nspace) {
    if (index.isValid()) {
        QFileInfo finfo = dirModel.fileInfo(index);
        if (finfo.exists() && finfo.isFile())
            finfo = dirModel.fileInfo(index.parent());

        QDir tmpDir;
        if (!catDir.isEmpty()) {
            QString pathWithNspace = dirPath + "/data/"
                                     + ((nspace.isEmpty()) ? Glhp::relNamespace(
                                            dirPath,
                                            finfo.filePath()) : nspace);
            tmpDir = QDir(pathWithNspace);
            if (!tmpDir.exists())
                return QModelIndex();

            pathWithNspace += '/' + catDir;
            if (!QFileInfo::exists(pathWithNspace))
                tmpDir.mkpath(pathWithNspace);
            tmpDir.cd(catDir);
        }

        QString newPath;
        if (catDir.isEmpty() || finfo.path().startsWith(tmpDir.path()))
            newPath = finfo.filePath() + "/" + name;
        else
            newPath = tmpDir.path() + "/" + name;
        QFile newFile(newPath);
        newFile.open(QIODevice::NewOnly);
        newFile.close();

        return dirModel.index(newPath);
    } else {
        return QModelIndex();
    }
}

void DatapackTreeView::contextMenuOnNew(const QString &name,
                                        const QString &catDir) {
    QModelIndex selected = indexAt(cMenuPos);
    QModelIndex index    = makeNewFile(selected, name, catDir);

    if (index.isValid()) {
        /*onDoubleClicked(index); */
        setCurrentIndex(index);
        edit(index);
    }
}

void DatapackTreeView::contextMenuOnOpen() {
    /*QModelIndex selected = getSelected(); */
    QModelIndex selected = indexAt(cMenuPos);

    if (selected.isValid()) {
        const QFileInfo finfo = dirModel.fileInfo(selected);
        /*qDebug() << finfo; */
        if (finfo.exists() && finfo.isFile()) {
/*            qDebug() << "Open from tree (right click)"; */
            emit openFileRequested(finfo.absoluteFilePath());
        }
    }
}

bool DatapackTreeView::isStringInTagFile(const QString &filepath,
                                         const QString &str) {
    if (!QFileInfo::exists(filepath)) return false;

    QFile inFile(filepath);
    inFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument doc = QJsonDocument::fromJson(inFile.readAll());
    inFile.close();
    QJsonObject root = doc.object();
    if (!root.isEmpty()) {
        if (root.contains("values") && root.value("values").isArray()) {
            auto values = root.value("values").toArray();
            return values.contains(str);
        }
    }
    return false;
}

void DatapackTreeView::contextMenuModifyTagFile(const QString &filepath,
                                                const QString &str,
                                                bool added) {
    QString errorMessage;

    if (!QFileInfo::exists(filepath)) {
        QDir().mkpath(QFileInfo(filepath).dir().path());
        QSaveFile newFile(filepath);
        if (newFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QJsonArray values;
            values.push_back(str);
            QJsonObject root;
            root.insert("values", values);
            QTextStream out(&newFile);
            out.setCodec("UTF-8");
            out << QJsonDocument(root).toJson();

            if (!newFile.commit()) {
                errorMessage = tr("Cannot write file %1:\n%2.")
                               .arg(QDir::toNativeSeparators(filepath),
                                    newFile.errorString());
            }
        } else{
            errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                           .arg(QDir::toNativeSeparators(filepath),
                                newFile.errorString());
        }

        if (!errorMessage.isEmpty()) {
            QMessageBox::information(this, tr("Error"), errorMessage);
        }
    }

    QFile file(filepath);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QJsonDocument doc  = QJsonDocument::fromJson(file.readAll());
        QJsonObject   root = doc.object();
        if (!root.isEmpty()) {
            if (root.contains("values") && root.value("values").isArray()) {
                auto values = root.value("values").toVariant().toStringList();
                if ((!values.contains(str)) && added) {
                    values.push_back(str);
                } else if (values.contains(str) && (!added)) {
                    values.removeAll(str);
                }
                root["values"] = QJsonArray::fromStringList(values);
                file.resize(0);
                QTextStream out(&file);
                out.setCodec("UTF-8");
                out << QJsonDocument(root).toJson();
            }
        }
    }
    file.close();
}

void DatapackTreeView::contextMenuOnRename() {
    /*QModelIndex selected = getSelected(); */
    QModelIndex selected = indexAt(cMenuPos);

    if (selected.isValid()) {
        edit(selected);
    }
}

void DatapackTreeView::contextMenuOnDelete() {
    /*QModelIndex selected = getSelected(); */
    QModelIndex &&selected = indexAt(cMenuPos);

    if (selected.isValid()) {
        QString &&filepath = dirModel.filePath(selected);
        dirModel.remove(selected);
        if (isStringInTagFile(dirPath +
                              "/data/minecraft/tags/functions/load.json",
                              Glhp::toNamespacedID(dirPath, filepath)))
            contextMenuModifyTagFile
                (dirPath + "/data/minecraft/tags/functions/load.json",
                Glhp::toNamespacedID(dirPath, filepath), false);
        if (isStringInTagFile(dirPath +
                              "/data/minecraft/tags/functions/tick.json",
                              Glhp::toNamespacedID(dirPath, filepath)))
            contextMenuModifyTagFile
                (dirPath + "/data/minecraft/tags/functions/tick.json",
                Glhp::toNamespacedID(dirPath, filepath), false);
        emit fileDeteted(filepath);
    }
}

void DatapackTreeView::contextMenuOnCopyId() {
    QModelIndex &&selected = indexAt(cMenuPos);

    if (selected.isValid()) {
        QString &&filepath = dirModel.filePath(selected);
        qApp->clipboard()->setText(Glhp::toNamespacedID(dirPath, filepath));
    }
}

void DatapackTreeView::resizeFirstColumn() {
    resizeColumnToContents(0);
}

void DatapackTreeView::onDoubleClicked(const QModelIndex &index) {
    /*qDebug() << "index.isValid: " << index.isValid(); */

    if (index.isValid()) {
        const QFileInfo finfo = dirModel.fileInfo(index);

        if (finfo.exists() && finfo.isFile()) {
            /*qDebug() << "Open from tree"; */
            setCurrentIndex(index);
            emit openFileRequested(finfo.filePath());
        }
    }
}

void DatapackTreeView::selectFromPath(const QString &path) {
    setCurrentIndex(dirModel.index(path));
}

void DatapackTreeView::openFromPath(const QString &path) {
    onDoubleClicked(dirModel.index(path));
}
