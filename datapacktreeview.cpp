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

    setupMenus();
}

void DatapackTreeView::setupMenus() { //Right click context menu

    QMenu *newMenu = new QMenu(tr("New"), this); //"New" menu

    QAction *newMenuNewFolder = new QAction(tr("Folder"), this);
    connect(newMenuNewFolder, &QAction::triggered, this, &DatapackTreeView::contextMenuOnNewFolder);
    newMenu->addAction(newMenuNewFolder);

    newMenu->addSeparator();

    QAction *newMenuNewFile = new QAction(tr("File"), this);
    connect(newMenuNewFile, &QAction::triggered, this, &DatapackTreeView::contextMenuOnNewFile);
    newMenu->addAction(newMenuNewFile);

    cMenu.addMenu(newMenu);

    QAction *cMenuActionOpen = new QAction(tr("Open"), this);
    cMenuActionOpen->setShortcuts(QKeySequence::Open);
    connect(cMenuActionOpen, &QAction::triggered, this, &DatapackTreeView::contextMenuOnOpen);
    cMenu.addAction(cMenuActionOpen);

    QAction *cMenuActionRename = new QAction(tr("Rename"), this);
    connect(cMenuActionRename, &QAction::triggered, this, &DatapackTreeView::contextMenuOnRename);
    cMenu.addAction(cMenuActionRename);

    QAction *cMenuActionDelete = new QAction(tr("Delete"), this);
    connect(cMenuActionDelete, &QAction::triggered, this, &DatapackTreeView::contextMenuOnDelete);
    cMenu.addAction(cMenuActionDelete);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &DatapackTreeView::customContextMenuRequested, this, &DatapackTreeView::onCustomContextMenu);

}

QModelIndex DatapackTreeView::getSelected() {
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();
    if (indexes.size() > 0)
        return indexes.at(0);
    else
        return QModelIndex();
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
    //qDebug() << "dragEnterEvent";
    //qDebug() << event->keyboardModifiers();
    //qDebug() << event->mimeData()->urls().at(0).toLocalFile();
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
    //qDebug() << event->dropAction();
    QTreeView::dragEnterEvent(event);
}

void DatapackTreeView::onCustomContextMenu(const QPoint &point) {
    QModelIndex index = this->indexAt(point);
    if (index.isValid()) {;
        cMenu.exec(this->viewport()->mapToGlobal(point));
    }
}

void DatapackTreeView::contextMenuOnNewFolder() {
    QModelIndex selected = getSelected();
    if(selected.isValid()) {
        const QFileInfo finfo = dirModel.fileInfo(selected);
        QModelIndex newDir;
        if (finfo.exists() && finfo.isFile())
            selected = selected.parent();

        const QString charset("abcdefghijklmnopqrstuvwxyz0123456789");
        QString randStr;
        for(int i = 0; i < 6; ++i) {
            int index = qrand() % charset.length();
            randStr.append(charset.at(index));
        }

        newDir = dirModel.mkdir(selected, "new_folder_"+randStr);
        edit(newDir);
    }
}

void DatapackTreeView::contextMenuOnNewFile() {
    QModelIndex selected = getSelected();
    if(selected.isValid()) {
        QFileInfo finfo = dirModel.fileInfo(selected);
        if (finfo.exists() && finfo.isFile())
            finfo = dirModel.fileInfo(selected.parent());

        const QString charset("abcdefghijklmnopqrstuvwxyz0123456789");
        QString randStr;
        for(int i = 0; i < 6; ++i) {
            int index = qrand() % charset.length();
            randStr.append(charset.at(index));
        }

        QString newPath = finfo.filePath() + "/new_file_"+randStr+".txt";
        QFile newFile(newPath);
        newFile.open(QIODevice::ReadWrite);
        newFile.close();

        edit(dirModel.index(newPath));
    }
}

void DatapackTreeView::contextMenuOnOpen() {
    QModelIndex selected = getSelected();
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
    QModelIndex selected = getSelected();
    if(selected.isValid()) {
        edit(selected);
    }
}

void DatapackTreeView::contextMenuOnDelete() {
    QModelIndex selected = getSelected();
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
            qobject_cast<MainWindow*>(this->parent()->parent()->parent())->openFile(finfo.absoluteFilePath());
        }
    }
}

void DatapackTreeView::selectFromPath(const QString &path) {
    setCurrentIndex(dirModel.index(path));
}
