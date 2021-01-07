#ifndef FILESWITCHER_H
#define FILESWITCHER_H

#include "tabbedcodeeditorinterface.h"

#include <QListWidget>

class FileSwitcher : public QListWidget
{
public:
    FileSwitcher(TabbedCodeEditorInterface *parent = nullptr);
    ~FileSwitcher();

protected:
    void focusOutEvent(QFocusEvent* event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void onSelectNextItem();
    void onSelectPrevItem();

private:
    TabbedCodeEditorInterface *parent = nullptr;
    void initFileList();
};

#endif /* FILESWITCHER_H */
