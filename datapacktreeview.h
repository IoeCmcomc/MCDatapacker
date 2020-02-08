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
    void openFromPath(const QString path);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;

private slots:
    void onDoubleClicked(const QModelIndex &index);
    void onCustomContextMenu(const QPoint &point);
    void contextMenuOnNewFolder();
    void contextMenuOnNewFunct();
    void contextMenuOnNewRecipe();
    void contextMenuOnNewFile();
    void contextMenuOnOpen();
    void contextMenuOnRename();
    void contextMenuOnDelete();

private:
    QFileSystemModel dirModel;
    QString dirPath;
    QPoint cMenuPos;
    QMenu *mkContextMenu(QModelIndex index);
    QModelIndex makeNewFile(QModelIndex index, QString name, QString catDir = "");
    QModelIndex getSelected();
    QString randStr(int length);
    QString relPath(QString path);
    QString relNamespace(QString path);
};

#endif // DATAPACKTREEVIEW_H
