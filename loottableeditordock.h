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
    void onAddPoll();
    void onDeletePoll();
    void onPollSelectionChanged();
    void checkPolls();

private:
    Ui::LootTableEditorDock *ui;
    QStandardItemModel model;
    QModelIndex curPollIndex;
    bool pollDeleted = false;
    QModelIndex indexBeforeDelete;


    void clearPollEditor();
    void getSelectedPoll();
    QJsonObject writePollJson();
    void readPollJson(const QJsonObject &root);
};

#endif /* LOOTTABLEEDITORDOCK_H */
