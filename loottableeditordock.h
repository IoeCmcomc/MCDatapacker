#ifndef LOOTTABLEEDITORDOCK_H
#define LOOTTABLEEDITORDOCK_H

#include <QDockWidget>
#include <QStandardItemModel>
#include <QJsonObject>
#include <QModelIndex>
#include <QHBoxLayout>

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
    void onCurrentPoolChanged(const QModelIndex &current,
                              const QModelIndex &previous);
    void onAddCondition();
    void onAddEntry();

    void checkPools();
    void reset();

private:
    Ui::LootTableEditorDock *ui;
    const QStringList types = {
        QStringLiteral("empty"),              QStringLiteral("entity"),
        QStringLiteral("block"),              QStringLiteral("chest"),
        QStringLiteral("fishing"),            QStringLiteral("gift"),
        QStringLiteral("advancement_reward"), QStringLiteral("generic")
    };
    QStandardItemModel model;
    bool poolDeleted = false;
    QModelIndex indexBeforeDelete;
    QHBoxLayout conditionsLayout;
    QHBoxLayout functionsLayout;
    QHBoxLayout entriesLayout;

    void clearPoolEditor();
    void getSelectedPool();
    QJsonObject writePoolJson();
    void readPoolJson(const QJsonObject &root);
};

#endif /* LOOTTABLEEDITORDOCK_H */
