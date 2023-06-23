#ifndef STATUSBAR_H
#define STATUSBAR_H

#include "codefile.h"

#include <QDir>
#include <QStatusBar>


class MainWindow;
class TabbedDocumentInterface;
class QLabel;
class ImgViewer;
class CodeEditor;

class StatusBar : public QStatusBar
{
    Q_OBJECT
public:
    StatusBar(MainWindow *parent                    = nullptr,
              TabbedDocumentInterface *tabInterface = nullptr);
    ~StatusBar();
    void onCurDirChanged();
    void onCurFileChanged();

public slots:
    void updateStatusFrom(QWidget *widget);

protected:
    void changeEvent(QEvent *e) override;

private:
    MainWindow *m_mainWin                      = nullptr;
    TabbedDocumentInterface *m_tabbedInterface = nullptr;
    QLabel *m_gameVerLabel                     = nullptr;
    QLabel *m_packFmtLabel                     = nullptr;
    QLabel *m_tabsLabel                        = nullptr;
    QLabel *m_fileLabel                        = nullptr;
    QVector<QLabel *> m_editorLabels;

    void updateCodeEditorStatus(CodeEditor *editor);
    void updateImgViewerStatus(ImgViewer *viewer);
};

#endif /* STATUSBAR_H */
