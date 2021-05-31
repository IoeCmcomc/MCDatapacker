#ifndef FILESWITCHER_H
#define FILESWITCHER_H

#include "tabbeddocumentinterface.h"

#include <QListWidget>

class FileSwitcher : public QListWidget
{
public:
    FileSwitcher(TabbedDocumentInterface *parent = nullptr,
                 bool backward                     = false);
    ~FileSwitcher();

protected:
    void focusOutEvent(QFocusEvent* event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void onSelectNextItem();
    void onSelectPrevItem();

private:
    TabbedDocumentInterface *parent = nullptr;
    void initFileList();
    void show();
};

#endif /* FILESWITCHER_H */
