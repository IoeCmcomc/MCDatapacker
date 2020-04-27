#ifndef LOOTTABLEEDITORDOCK_H
#define LOOTTABLEEDITORDOCK_H

#include <QDockWidget>
#include <QStandardItemModel>
#include <QJsonObject>
#include <QModelIndex>

namespace Ui {
    class LootTableEditorDock;
}

class LootTableEditorDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit LootTableEditorDock(QWidget *parent = nullptr);
    ~LootTableEditorDock();

    void writeJson();

protected slots:
    void onAddPool();
    void onDeletePool();
    void onPoolSelectionChanged();
    void checkPools();

private:
    Ui::LootTableEditorDock *ui;
    QStandardItemModel model;
    QModelIndex curPoolIndex;
    bool poolDeleted = false;
    QModelIndex indexBeforeDelete;


    void clearPoolEditor();
    void getSelectedPool();
    QJsonObject writePoolJson();
    void readPoolJson(const QJsonObject &root);
};

#endif /* LOOTTABLEEDITORDOCK_H */
