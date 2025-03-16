#ifndef VANILLADATAPACKDOCK_H
#define VANILLADATAPACKDOCK_H

#include "vanilladatapackmodel.h"

#include <QDockWidget>

namespace Ui {
    class VanillaDatapackDock;
}

/*!
 * \class VanillaDatapackDock
 * \brief A dock widget containing a tree view of default datapack files.
 */
class VanillaDatapackDock : public QDockWidget {
    Q_OBJECT

public:
    explicit VanillaDatapackDock(QWidget *parent = nullptr);
    ~VanillaDatapackDock();

public: // slots
    void setPackOpened(const bool value);

signals:
    void openFileRequested(const QString &path, const QString &actualPath = {});

protected:
    void changeEvent(QEvent *e);

private: // Slots
    void onDoubleClicked(const QModelIndex &index);
    void onCustomContextMenu(const QPoint &pos);

private:
    VanillaDatapackModel m_model;
    Ui::VanillaDatapackDock *ui;
    bool m_packOpened = false;

    QString realPath(const QModelIndex &index) const;
    void duplicateFile(const QModelIndex &index, const QString &nspace);
};

#endif // VANILLADATAPACKDOCK_H
