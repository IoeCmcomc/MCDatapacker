#ifndef LOOTTABLEEDITORDOCK_H
#define LOOTTABLEEDITORDOCK_H

#include <QDockWidget>

class MainWindow;

namespace Ui {
    class LootTableEditorDock;
}

class LootTableEditorDock : public QDockWidget {
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
    MainWindow *m_mainWin = nullptr;
};

#endif /* LOOTTABLEEDITORDOCK_H */
