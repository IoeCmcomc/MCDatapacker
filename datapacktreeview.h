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

protected:

private slots:
    void onDoubleClicked(const QModelIndex &index);
    void onCustomContextMenu(const QPoint &point);
    void contextMenuOnOpen();

private:
    QFileSystemModel dirModel;
    QMenu cMenu;
    QAction *cMenuActionOpen;


};

#endif // DATAPACKTREEVIEW_H
