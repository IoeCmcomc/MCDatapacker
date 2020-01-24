#include "datapacktreeview.h"
#include "mainwindow.h"

#include <QDebug>
#include <QModelIndex>
#include <QFile>

DatapackTreeView::DatapackTreeView(QWidget *parent) : QTreeView(parent)
{
    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));

    cMenuActionOpen = new QAction(tr("Open"), this);
    //cMenuActionOpen->setShortcuts();

    cMenu.addAction(cMenuActionOpen);
    connect(cMenuActionOpen, &QAction::triggered, this, &DatapackTreeView::contextMenuOnOpen);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &DatapackTreeView::customContextMenuRequested, this, &DatapackTreeView::onCustomContextMenu);
}

void DatapackTreeView::load(const QString &dir) {
    //dirModel.setRootPath(dir);
    dirModel.setRootPath("");

    setModel(&dirModel);

    for (int i = 1; i < dirModel.columnCount(); ++i)
        hideColumn(i);

    setRootIndex(dirModel.index(dir));
}

void DatapackTreeView::onCustomContextMenu(const QPoint &point) {
    QModelIndex index = this->indexAt(point);
    if (index.isValid()) {
        cMenu.exec(this->viewport()->mapToGlobal(point));
    }
}

void DatapackTreeView::contextMenuOnOpen() {
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();
    if (indexes.size() > 0) {
        QModelIndex selectedIndex = indexes.at(0);
        if(selectedIndex.isValid()) {
            const QFileInfo finfo = dirModel.fileInfo(selectedIndex);
            //qDebug() << finfo;
            if (finfo.exists() && finfo.isFile()) {
                //qDebug() << "Open from tree (right click)";
                qobject_cast<MainWindow*>(this->parent()->parent()->parent())->openFile(finfo.absoluteFilePath());
            }
        }
    }
}

void DatapackTreeView::onDoubleClicked(const QModelIndex &index) {
    qDebug() << "index.isValid: " << index.isValid();

    if(index.isValid()) {
        const QFileInfo finfo = dirModel.fileInfo(index);
        //qDebug() << finfo;

        if (finfo.exists() && finfo.isFile()) {
            //qDebug() << "Open from tree";
            qobject_cast<MainWindow*>(this->parent()->parent()->parent())->openFile(finfo.absoluteFilePath());
        }
    }
}
