#ifndef STATUSBAR_H
#define STATUSBAR_H

#include "codefile.h"

#include <QDir>
#include <QStatusBar>

class MainWindow;
class TabbedCodeEditorInterface;
class QLabel;
class ImgViewer;

class StatusBar : public QStatusBar
{
public:
    StatusBar(MainWindow *parent                      = nullptr,
              TabbedCodeEditorInterface *tabInterface = nullptr);

    void onCurDirChanged();
    void onCurFileChanged();

public slots:
    void updateCodeEditorStatus(CodeEditor *editor);
    void updateImgViewerStatus(ImgViewer *viewer);

private:
    MainWindow *m_mainWin                        = nullptr;
    TabbedCodeEditorInterface *m_tabbedInterface = nullptr;
    QLabel *m_gameVerLabel                       = nullptr;
    QLabel *m_packFmtLabel                       = nullptr;
    QLabel *m_tabsLabel                          = nullptr;
    QLabel *m_fileLabel                          = nullptr;
    QVector<QLabel*> m_editorLabels;

    static QString fileTypeToString(CodeFile::FileType type);
};

#endif /* STATUSBAR_H */
