#include "datapacktreeview.h"

#include "filenamedelegate.h"

#include "globalhelpers.h"
#include "game.h"

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
#include <QMessageBox>
#include <QInputDialog>

DatapackTreeView::DatapackTreeView(QWidget *parent) : QTreeView(parent) {
    dirModel.setReadOnly(false);
    dirModel.setIconProvider(&iconProvider);

    setItemDelegateForColumn(0, new FileNameDelegate(this));

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
    auto           *cMenu = new QMenu(this);
    const QString &&path  = Glhp::relPath(dirPath, dirModel.filePath(index));
    QFileInfo     &&finfo = dirModel.fileInfo(index);

    if (finfo.isFile()) {
        QAction *cMenuActionOpen = new QAction(tr("Open"), cMenu);
        cMenuActionOpen->setShortcuts(QKeySequence::Open);
        connect(cMenuActionOpen, &QAction::triggered,
                this, &DatapackTreeView::contextMenuOnOpen);
        cMenu->addAction(cMenuActionOpen);
    }

    const auto &&fileType = Glhp::pathToFileType(dirPath, finfo.filePath());
    if ((fileType == CodeFile::Function || fileType >= CodeFile::JsonText) &&
        path.startsWith(QLatin1String("data/"))) {
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


    if (path != QLatin1String("data") &&
        path != QLatin1String("pack.mcmeta")) {
        QAction *cMenuActionRename = new QAction(tr("Rename"), cMenu);
        connect(cMenuActionRename, &QAction::triggered,
                this, &DatapackTreeView::contextMenuOnRename);
        cMenu->addAction(cMenuActionRename);

        QAction *cMenuActionDelete = new QAction(tr("Delete"), cMenu);
        connect(cMenuActionDelete, &QAction::triggered,
                this, &DatapackTreeView::contextMenuOnDelete);
        cMenu->addAction(cMenuActionDelete);
    }

    if (path != QLatin1String("pack.mcmeta")) {
        cMenu->addSeparator();
        QMenu *newMenu = new QMenu(tr("New"), cMenu); /*"New" menu */

        QAction *newMenuNewFolder = new QAction(tr("Folder"), newMenu);
        if (path == QLatin1String("data"))
            newMenuNewFolder->setText(tr("Namespace"));
        connect(newMenuNewFolder, &QAction::triggered,
                this, &DatapackTreeView::contextMenuOnNewFolder);
        newMenu->addAction(newMenuNewFolder);

        if (path.contains('/')) {
            const static QLatin1String jsonExt(".json");
            newMenu->addSeparator();

            addNewFileAction(newMenu, tr("Advancement"), jsonExt,
                             QLatin1String("advancements"));
            if (Game::version() >= Game::v1_16) {
                addNewFileAction(newMenu, tr("Dimension"), jsonExt,
                                 QLatin1String("dimension"));
                addNewFileAction(newMenu, tr("Dimension type"), jsonExt,
                                 QLatin1String("type"));
            }
            addNewFileAction(newMenu, tr("Function"),
                             QLatin1String(".mcfunction"),
                             QLatin1String("functions"));
            addNewFileAction(newMenu, tr("Loot table"), jsonExt,
                             QLatin1String("loot_tables"));
            if (Game::version() >= Game::v1_17) {
                addNewFileAction(newMenu, tr("Item modifier"), jsonExt,
                                 QLatin1String("item_modifiers"));
            }
            addNewFileAction(newMenu, tr("Predicate"), jsonExt,
                             QLatin1String("predicates"));
            addNewFileAction(newMenu, tr("Recipe"), jsonExt,
                             QLatin1String("recipes"));
            addNewFileAction(newMenu, tr("Structure"),
                             QLatin1String(".nbt"),
                             QLatin1String("structures"));

            /*"Tag" menu */
            QMenu *tagMenu = new QMenu(tr("Tag"), newMenu);

            addNewFileAction(tagMenu, tr("Blocks"), jsonExt,
                             QLatin1String("tags/blocks"));
            addNewFileAction(tagMenu, tr("Entity types"), jsonExt,
                             QLatin1String("tags/entity_types"));
            addNewFileAction(tagMenu, tr("Fluids"), jsonExt,
                             QLatin1String("tags/fluids"));
            addNewFileAction(tagMenu, tr("Functions"), jsonExt,
                             QLatin1String("tags/functions"));
            if (Game::version() >= Game::v1_17) {
                addNewFileAction(tagMenu, tr("Game events"), jsonExt,
                                 QLatin1String("tags/game_events"));
            }
            addNewFileAction(tagMenu, tr("Items"), jsonExt,
                             QLatin1String("tags/items"));

            if (Game::version() >= Game::v1_18_2) {
                QAction *action = new QAction(tr("Other..."), tagMenu);

                connect(action, &QAction::triggered, this, [this]() {
                    bool ok;
                    const QString &tagType = QInputDialog::getText(this, tr("Tag folder path"),
                                            tr("Please type the folder path of the tag (without the 'tags' prefix):"),
                                            QLineEdit::Normal, QString(), &ok);
                    if (ok && !tagType.isEmpty()) {
                        this->contextMenuOnNew(QLatin1String(".json"), QStringLiteral("tags/") + tagType);
                    }
                });
                tagMenu->addAction(action);
            }

            newMenu->addMenu(tagMenu);

            if (Game::version() >= Game::v1_16) {
                /*"World generation" menu */
                QMenu *worldMenu = new QMenu(tr("World generation"), newMenu);

                addNewFileAction(worldMenu, tr("Biome"), jsonExt,
                                 QLatin1String("worldgen/biome"));
                addNewFileAction(worldMenu, tr("Carver"), jsonExt,
                                 QLatin1String("worldgen/configured_carver"));
                addNewFileAction(worldMenu, tr("Feature"), jsonExt,
                                 QLatin1String("worldgen/configured_feature"));
                addNewFileAction(worldMenu, tr("Structure feature"), jsonExt,
                                 QLatin1String(
                                     "worldgen/configured_structure_feature"));
                if (Game::version() <= Game::v1_17) {
                    addNewFileAction(worldMenu, tr("Surface builder"), jsonExt,
                                     QLatin1String(
                                         "worldgen/configured_surface_builder"));
                } else {
                    addNewFileAction(worldMenu, tr("Noise"), jsonExt,
                                     QLatin1String("worldgen/noise"));
                }
                addNewFileAction(worldMenu, tr("Noise settings"), jsonExt,
                                 QLatin1String("worldgen/noise_settings"));
                if (Game::version() >= Game::v1_18) {
                    addNewFileAction(worldMenu, tr("Placed feature"), jsonExt,
                                     QLatin1String("worldgen/placed_feature"));
                }
                addNewFileAction(worldMenu, tr("Processor list"), jsonExt,
                                 QLatin1String("worldgen/processor_list"));
                if (Game::version() >= Game::v1_18_2) {
                    addNewFileAction(worldMenu, tr("Structure set"), jsonExt,
                                     QLatin1String("worldgen/structure_set"));
                }
                addNewFileAction(worldMenu, tr("Jigsaw pool"), jsonExt,
                                 QLatin1String("worldgen/template_pool"));

                newMenu->addMenu(worldMenu);
            }

            newMenu->addSeparator();

            addNewFileAction(newMenu, tr("Text file"), QLatin1String(".txt"));
        }
        cMenu->addMenu(newMenu);
    }

    return cMenu;
}

QAction *DatapackTreeView::addNewFileAction(QMenu *menu, const QString &name,
                                            QLatin1String ext,
                                            QLatin1String catDir) {
    QAction *action = new QAction(name, menu);

    connect(action, &QAction::triggered, this, [this, ext, catDir]() {
        this->contextMenuOnNew(ext, catDir);
    });
    menu->addAction(action);
    return action;
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
            QString pathWithNspace = dirPath + QStringLiteral("/data/")
                                     + ((nspace.isEmpty()) ? Glhp::relNamespace(
                                            dirPath,
                                            finfo.filePath()) : nspace);
            tmpDir.setPath(pathWithNspace);
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

void DatapackTreeView::contextMenuOnNew(const QString &ext,
                                        const QString &catDir) {
    QModelIndex selected = indexAt(cMenuPos);
    QModelIndex index    = makeNewFile(selected, Glhp::randStr() + ext, catDir);

    if (index.isValid()) {
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
