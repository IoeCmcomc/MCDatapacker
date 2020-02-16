#include "datapacktreeview.h"
#include "mainwindow.h"

#include <QDebug>
#include <QModelIndex>
#include <QFile>
#include <QDropEvent>
#include <QMimeData>


DatapackTreeView::DatapackTreeView(QWidget *parent) : QTreeView(parent)
{
    dirModel.setReadOnly(false);

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
    //connect(this, &QTreeView::clicked, this, &DatapackTreeView::onDoubleClicked);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &DatapackTreeView::customContextMenuRequested, this, &DatapackTreeView::onCustomContextMenu);
}

QMenu *DatapackTreeView::mkContextMenu(QModelIndex index) {

    QMenu *cMenu = new QMenu(this); //Right click context menu
    QString path = relPath(dirModel.filePath(index));
    qDebug() << path;
    QFileInfo finfo = dirModel.fileInfo(index);

    if(finfo.isFile()) {
        QAction *cMenuActionOpen = new QAction(tr("Open"), this);
        cMenuActionOpen->setShortcuts(QKeySequence::Open);
        connect(cMenuActionOpen, &QAction::triggered, this, &DatapackTreeView::contextMenuOnOpen);
        cMenu->addAction(cMenuActionOpen);
    }

    if(path != "data" && path != "pack.mcmeta") {
        QAction *cMenuActionRename = new QAction(tr("Rename"), this);
        connect(cMenuActionRename, &QAction::triggered, this, &DatapackTreeView::contextMenuOnRename);
        cMenu->addAction(cMenuActionRename);

        QAction *cMenuActionDelete = new QAction(tr("Delete"), this);
        connect(cMenuActionDelete, &QAction::triggered, this, &DatapackTreeView::contextMenuOnDelete);
        cMenu->addAction(cMenuActionDelete);
    }

    if(path != "pack.mcmeta") {
        cMenu->addSeparator();
        QMenu *newMenu = new QMenu(tr("New"), this); //"New" menu

        QAction *newMenuNewFolder = new QAction(tr("Folder"), this);
        if(path == "data")
            newMenuNewFolder->setText(tr("Namespace"));
        connect(newMenuNewFolder, &QAction::triggered, this, &DatapackTreeView::contextMenuOnNewFolder);
        newMenu->addAction(newMenuNewFolder);

        if(path != "data") {
            newMenu->addSeparator();

            QAction *newMenuNewAdv = new QAction(tr("Advancement"), this);
            connect(newMenuNewAdv, &QAction::triggered, this, &DatapackTreeView::contextMenuOnNewAdv);
            newMenu->addAction(newMenuNewAdv);

            QAction *newMenuNewFunct = new QAction(tr("Function"), this);
            connect(newMenuNewFunct, &QAction::triggered, this, &DatapackTreeView::contextMenuOnNewFunct);
            newMenu->addAction(newMenuNewFunct);

            QAction *newMenuNewLoot = new QAction(tr("Loot table"), this);
            connect(newMenuNewLoot, &QAction::triggered, this, &DatapackTreeView::contextMenuOnNewLoot);
            newMenu->addAction(newMenuNewLoot);

            QAction *newMenuNewPred = new QAction(tr("Predicate"), this);
            connect(newMenuNewPred, &QAction::triggered, this, &DatapackTreeView::contextMenuOnNewPred);
            newMenu->addAction(newMenuNewPred);

            QAction *newMenuNewRecipe = new QAction(tr("Recipe"), this);
            connect(newMenuNewRecipe, &QAction::triggered, this, &DatapackTreeView::contextMenuOnNewRecipe);
            newMenu->addAction(newMenuNewRecipe);

            QAction *newMenuNewStruct = new QAction(tr("Structure"), this);
            newMenuNewStruct->setDisabled(true);
            connect(newMenuNewStruct, &QAction::triggered, this, &DatapackTreeView::contextMenuOnNewStruct);
            newMenu->addAction(newMenuNewStruct);

            QMenu *tagMenu = new QMenu(tr("Tag"), this); //"Tag" menu

            QAction *newBlockTag = new QAction(tr("Blocks"), this);
            connect(newBlockTag, &QAction::triggered, this, &DatapackTreeView::contextMenuOnNewBlocksTag);
            tagMenu->addAction(newBlockTag);

            QAction *newEntityTag = new QAction(tr("Entity types"), this);
            connect(newEntityTag, &QAction::triggered, this, &DatapackTreeView::contextMenuOnNewEntityTag);
            tagMenu->addAction(newEntityTag);

            QAction *newFunctTag = new QAction(tr("Functions"), this);
            connect(newFunctTag, &QAction::triggered, this, &DatapackTreeView::contextMenuOnNewFunctsTag);
            tagMenu->addAction(newFunctTag);

            QAction *newItemTag = new QAction(tr("Items"), this);
            connect(newItemTag, &QAction::triggered, this, &DatapackTreeView::contextMenuOnNewItemsTag);
            tagMenu->addAction(newItemTag);

            newMenu->addMenu(tagMenu);

            newMenu->addSeparator();

            QAction *newMenuNewFile = new QAction(tr("File"), this);
            connect(newMenuNewFile, &QAction::triggered, this, &DatapackTreeView::contextMenuOnNewFile);
            newMenu->addAction(newMenuNewFile);
        }
        cMenu->addMenu(newMenu);
    }

    return cMenu;
}

QModelIndex DatapackTreeView::getSelected() {
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();
    return (indexes.size() > 0) ? indexes.at(0) : QModelIndex();
}

void DatapackTreeView::load(const QString &dir) {
    //dirModel.setRootPath(dir);
    dirModel.setRootPath("");
    setModel(&dirModel);

    for (int i = 1; i < dirModel.columnCount(); ++i)
        hideColumn(i);

    setRootIndex(dirModel.index(dir));

    dirPath = dir;
}


void DatapackTreeView::dragEnterEvent(QDragEnterEvent *event) {
    QModelIndex draggedIndex = dirModel.index(
                event->mimeData()->urls().at(0).toLocalFile()
                );
    if(event->keyboardModifiers() & Qt::ShiftModifier
            || draggedIndex == dirModel.index(dirPath+"/data")
            || draggedIndex == dirModel.index(dirPath+"/pack.mcmeta")) {
        event->setDropAction(Qt::IgnoreAction);
        event->ignore();
    } else if(event->keyboardModifiers() & Qt::ControlModifier) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    QTreeView::dragEnterEvent(event);
}

QString DatapackTreeView::relPath(QString path) {
    QString dataDir = dirPath + "/";
    if(path.startsWith(dataDir)) {
        path = path.remove(0, dataDir.length());
    } else {
        return "";
    }
    return path;
}

QString DatapackTreeView::relNamespace(QString path) {
    QString rp = relPath(path);
    if(rp.startsWith("data/")) {
        rp.remove(0, 5);
        rp = rp.section('/', 0, 0);
    } else {
        rp = "";
    }
    return rp;
}

QString DatapackTreeView::randStr(int length = 5) {
    const QString charset("abcdefghijklmnopqrstuvwxyz0123456789");
    QString r;
    for(int i = 0; i < (length + 1); ++i) {
        int index = qrand() % charset.length();
        r.append(charset.at(index));
    }
    return r;
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

void DatapackTreeView::contextMenuOnNewFolder() {
    //QModelIndex selected = getSelected();
    QModelIndex selected = indexAt(cMenuPos);
    if(selected.isValid()) {
        const QFileInfo finfo = dirModel.fileInfo(selected);
        QModelIndex newDir;
        if (finfo.exists() && finfo.isFile())
            selected = selected.parent();
        newDir = dirModel.mkdir(selected, "new_folder_"+randStr());
        setCurrentIndex(newDir);
        edit(newDir);
    }
}

QModelIndex DatapackTreeView::makeNewFile(QModelIndex index, QString name,
                                          QString catDir) {
    if(index.isValid()) {
        QFileInfo finfo = dirModel.fileInfo(index);
        if (finfo.exists() && finfo.isFile())
            finfo = dirModel.fileInfo(index.parent());

        QDir tmpDir;
        if(!catDir.isEmpty()) {
            QString pathWithNspace = dirPath+"/data/"+relNamespace(finfo.filePath());
            tmpDir= QDir(pathWithNspace);
            if(!tmpDir.exists())
                return QModelIndex();

            pathWithNspace += '/' + catDir;
            if(!QFileInfo::exists(pathWithNspace))
                tmpDir.mkpath(pathWithNspace);
            tmpDir.cd(catDir);
        }

        QString newPath;
        if(catDir.isEmpty() || finfo.path().startsWith(tmpDir.path()))
            newPath = finfo.filePath() + "/" + name;
        else
            newPath = tmpDir.path() + "/" + name;
        qDebug() << newPath;
        QFile newFile(newPath);
        newFile.open(QIODevice::ReadWrite);
        newFile.close();

        return dirModel.index(newPath);
    } else {
        return QModelIndex();
    }
}

void DatapackTreeView::contextMenuOnNewAdv() {
    QModelIndex selected = indexAt(cMenuPos);
    QModelIndex index = makeNewFile(selected, "advancement_"+randStr()+".json", "advancements");
    if(index.isValid()) {
        onDoubleClicked(index);
        edit(index);
    }
}

void DatapackTreeView::contextMenuOnNewFunct() {
    //QModelIndex selected = getSelected();
    QModelIndex selected = indexAt(cMenuPos);
    QModelIndex index = makeNewFile(selected, "function_"+randStr()+".mcfunction", "functions");
    if(index.isValid()) {
        onDoubleClicked(index);
        edit(index);
    }
}

void DatapackTreeView::contextMenuOnNewLoot() {
    QModelIndex selected = indexAt(cMenuPos);
    QModelIndex index = makeNewFile(selected, "loot_table_"+randStr()+".json", "loot_tables");
    if(index.isValid()) {
        onDoubleClicked(index);
        edit(index);
    }
}

void DatapackTreeView::contextMenuOnNewPred() {
    QModelIndex selected = indexAt(cMenuPos);
    QModelIndex index = makeNewFile(selected, "predicate_"+randStr()+".json", "predicates");
    if(index.isValid()) {
        onDoubleClicked(index);
        edit(index);
    }
}

void DatapackTreeView::contextMenuOnNewRecipe() {
    //QModelIndex selected = getSelected();
    QModelIndex selected = indexAt(cMenuPos);
    QModelIndex index = makeNewFile(selected, "recipe_"+randStr()+".json", "recipes");
    if(index.isValid()) {
        onDoubleClicked(index);
        edit(index);
    }
}

void DatapackTreeView::contextMenuOnNewStruct() {
    //QModelIndex selected = getSelected();
    QModelIndex selected = indexAt(cMenuPos);
    QModelIndex index = makeNewFile(selected, "structure_"+randStr()+".nbt", "structures");
    if(index.isValid()) {
        onDoubleClicked(index);
        edit(index);
    }
}

void DatapackTreeView::contextMenuOnNewBlocksTag() {
    QModelIndex selected = indexAt(cMenuPos);
    QModelIndex index = makeNewFile(selected, "tag_"+randStr()+".json", "tags/blocks");
    if(index.isValid()) {
        onDoubleClicked(index);
        edit(index);
    }
}

void DatapackTreeView::contextMenuOnNewEntityTag() {
    QModelIndex selected = indexAt(cMenuPos);
    QModelIndex index = makeNewFile(selected, "tag_"+randStr()+".json", "tags/entity_types");
    if(index.isValid()) {
        onDoubleClicked(index);
        edit(index);
    }
}

void DatapackTreeView::contextMenuOnNewFunctsTag() {
    QModelIndex selected = indexAt(cMenuPos);
    QModelIndex index = makeNewFile(selected, "tag_"+randStr()+".json", "tags/functions");
    if(index.isValid()) {
        onDoubleClicked(index);
        edit(index);
    }
}

void DatapackTreeView::contextMenuOnNewItemsTag() {
    QModelIndex selected = indexAt(cMenuPos);
    QModelIndex index = makeNewFile(selected, "tag_"+randStr()+".json", "tags/items");
    if(index.isValid()) {
        onDoubleClicked(index);
        edit(index);
    }
}

void DatapackTreeView::contextMenuOnNewFile() {
    //QModelIndex selected = getSelected();
    QModelIndex selected = indexAt(cMenuPos);
    QModelIndex index = makeNewFile(selected, "file_"+randStr()+".txt");
    if(index.isValid()) {
        onDoubleClicked(index);
        edit(index);
    }
}

void DatapackTreeView::contextMenuOnOpen() {
    //QModelIndex selected = getSelected();
    QModelIndex selected = indexAt(cMenuPos);
    if(selected.isValid()) {
        const QFileInfo finfo = dirModel.fileInfo(selected);
        //qDebug() << finfo;
        if (finfo.exists() && finfo.isFile()) {
            //qDebug() << "Open from tree (right click)";
            qobject_cast<MainWindow*>(this->parent()->parent()->parent())->openFile(finfo.absoluteFilePath());
        }
    }
}

void DatapackTreeView::contextMenuOnRename() {
    //QModelIndex selected = getSelected();
    QModelIndex selected = indexAt(cMenuPos);
    if(selected.isValid()) {
        edit(selected);
    }
}

void DatapackTreeView::contextMenuOnDelete() {
    //QModelIndex selected = getSelected();
    QModelIndex selected = indexAt(cMenuPos);
    if(selected.isValid()) {
        dirModel.remove(selected);
    }
}

void DatapackTreeView::onDoubleClicked(const QModelIndex &index) {
    //qDebug() << "index.isValid: " << index.isValid();

    if(index.isValid()) {
        const QFileInfo finfo = dirModel.fileInfo(index);
        if (finfo.exists() && finfo.isFile()) {
            //qDebug() << "Open from tree";
            setCurrentIndex(index);
            qobject_cast<MainWindow*>(this->parent()->parent()->parent())->openFile(finfo.absoluteFilePath());
        }
    }
}

void DatapackTreeView::selectFromPath(const QString &path) {
    setCurrentIndex(dirModel.index(path));
}

void DatapackTreeView::openFromPath(const QString path) {
    onDoubleClicked(dirModel.index(path));
}
