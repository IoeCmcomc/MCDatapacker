#ifndef DATAPACKTREEVIEW_H
#define DATAPACKTREEVIEW_H

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

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;

private slots:
    void onDoubleClicked(const QModelIndex &index);
    void onCustomContextMenu(const QPoint &point);
    void contextMenuOnNewFolder();
    void contextMenuOnNewFile();
    void contextMenuOnOpen();
    void contextMenuOnRename();
    void contextMenuOnDelete();

private:
    QFileSystemModel dirModel;
    QString dirPath;
    QMenu cMenu;
    void setupMenus();
    QModelIndex getSelected();
};

#endif // DATAPACKTREEVIEW_H
