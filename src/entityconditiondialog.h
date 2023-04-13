#ifndef ENTITYCONDITIONDIALOG_H
#define ENTITYCONDITIONDIALOG_H

#include "basecondition.h"

#include <QDialog>

namespace Ui {
    class EntityConditionDialog;
}

class EntityConditionDialog : public QDialog, public BaseCondition
{
    Q_OBJECT

public:
    explicit EntityConditionDialog(QWidget *parent = nullptr);
    ~EntityConditionDialog();

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &value) override;

private slots:
    void onAddedEntityEffect();
    void onAddedPlayerAdv();
    void onAddedPlayerRecipe();
    void onAddedPlayerStat();
    void onEntityTypeChanged();

private:
    Ui::EntityConditionDialog *ui;
    QStandardItemModel entityModel;
    QStandardItemModel effectModel;
    QStandardItemModel entityEffectModel;
    QStandardItemModel playerRecipeModel;
    QStandardItemModel playerAdvanModel;
    QStandardItemModel statTypeModel;
    QStandardItemModel playerStatModel;

    void initEffectsPage();
    void initPlayerAdv();
    void initPlayerRecipe();
    void initPlayerStat();
    void setupGrantedTableFromJson(const QJsonObject &json,
                                   QStandardItemModel &model);
    void playerFromJson(const QJsonObject &player);
};

#endif /* ENTITYCONDITIONDIALOG_H */
