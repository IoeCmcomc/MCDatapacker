#ifndef ADVANCEMENTTABDOCK_H
#define ADVANCEMENTTABDOCK_H

#include <QDockWidget>
#include <QJsonValue>

struct AdvancemDisplayInfo {
    enum class FrameType: char {
        Task,
        Goal,
        Challenge,
    };

    QPixmap                                displayIcon;
    std::map<QString, AdvancemDisplayInfo> children;
    QJsonValue                             title;
    QJsonValue                             description;
    QString                                backgroundPath;
    QString                                parent;
    FrameType                              frameType = FrameType::Task;
    bool                                   missing   = false;
    bool                                   hidden    = false;
};

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

    bool m_pendingLoad = false;
};

#endif // ADVANCEMENTTABDOCK_H
