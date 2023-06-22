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
    void readJson();

protected:
    void changeEvent(QEvent *event) override;

private:
    Ui::LootTableEditorDock *ui;
    const QStringList types = {
        QStringLiteral("empty"),                QStringLiteral("entity"),
        QStringLiteral("block"),                QStringLiteral("chest"),
        QStringLiteral("fishing"),              QStringLiteral("gift"),
        QStringLiteral("advancement_reward"),   QStringLiteral("generic"),
        QStringLiteral("barter"),               QStringLiteral("command"),
        QStringLiteral("selector"),             QStringLiteral(
            "advancement_entity"),              QStringLiteral("archaelogy"),
        QStringLiteral("advancement_location"),
    };

private /*slots*/ :
    void updatePoolsTab(int size);
    void updateFunctionsTab(int size);
};

#endif /* LOOTTABLEEDITORDOCK_H */
