#ifndef ITEMMODIFIERDOCK_H
#define ITEMMODIFIERDOCK_H

#include <QDockWidget>

namespace Ui {
    class ItemModifierDock;
}

class ItemModifierDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit ItemModifierDock(QWidget *parent = nullptr);
    ~ItemModifierDock();

protected:
    void changeEvent(QEvent *event) override;

protected slots:
    void onReadBtn();
    void onWriteBtn();

private:
    Ui::ItemModifierDock *ui;
};

#endif /* ITEMMODIFIERDOCK_H */
