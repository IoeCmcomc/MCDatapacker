#ifndef ADVANCEMENTTABDOCK_H
#define ADVANCEMENTTABDOCK_H

#include "advancementtab.h"

#include <QDockWidget>
#include <QJsonValue>

namespace Ui {
    class AdvancementTabDock;
}

class AdvancementTabDock : public QDockWidget {
    Q_OBJECT

public:
    explicit AdvancementTabDock(QWidget *parent = nullptr);
    ~AdvancementTabDock();

    void loadAdvancements();

signals:
    void openFileRequested(const QString &filePath);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AdvancementTabDock *ui;

    QMap<QString, AdvancementTab *> m_tabs;
    bool m_pendingLoad = false;
};

#endif // ADVANCEMENTTABDOCK_H
