#ifndef FILESWITCHER_H
#define FILESWITCHER_H

#include "tabbeddocumentinterface.h"

#include <QListWidget>

class FileSwitcher : public QListWidget {
public:
    explicit FileSwitcher(TabbedDocumentInterface *parent = nullptr,
                          bool backward                   = false);
    ~FileSwitcher();

protected:
    QSize sizeHint() const final;
    void focusOutEvent(QFocusEvent *event) final;
    void keyReleaseEvent(QKeyEvent *event) final;

private slots:
    void onSelectNextItem();
    void onSelectPrevItem();

private:
    TabbedDocumentInterface *parent = nullptr;
    void initFileList();
    void show();
};

#endif /* FILESWITCHER_H */
