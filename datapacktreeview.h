#ifndef DATAPACKTREEVIEW_H
#define DATAPACKTREEVIEW_H

#include "datapackfileiconprovider.h"

#include <QTreeView>
#include <QFileSystemModel>
#include <QMenu>
#include <QAction>

class DatapackTreeView : public QTreeView
{
    Q_OBJECT
public:
    DatapackTreeView(QWidget *parent = nullptr);
    void load(const QString &dir);
    void selectFromPath(const QString &path);
    void openFromPath(const QString &path);

signals:
    void datapackChanged();
    void openFileRequested(const QString &path);
    void fileRenamed(const QString &path, const QString &oldName,
                     const QString &newName);
    void fileDeteted(const QString &path);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;

private slots:
    void onDoubleClicked(const QModelIndex &index);
    void onFileRenamed(const QString &path, const QString &oldName,
                       const QString &newName);
    void onCustomContextMenu(const QPoint &point);
    void contextMenuOnNewFolder();
    void contextMenuOnNew(const QString &name, const QString &catDir = "");
    void contextMenuOnOpen();
    void contextMenuOnRename();
    void contextMenuOnDelete();

private:
    QFileSystemModel dirModel;
    QString dirPath;
    QPoint cMenuPos;
    DatapackFileIconProvider iconProvider;

    QMenu *mkContextMenu(QModelIndex index);
    QModelIndex makeNewFile(QModelIndex index,
                            const QString &name,
                            const QString &catDir = "",
                            const QString &nspace = "");
    QModelIndex getSelected();
    bool isStringInTagFile(const QString &filepath, const QString &str);
    void contextMenuModifyTagFile(const QString &filepath, const QString &str,
                                  bool added = true);
};

#endif /* DATAPACKTREEVIEW_H */
