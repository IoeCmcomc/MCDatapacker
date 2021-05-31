#ifndef TABBEDCODEEDITORINTERFACE_H
#define TABBEDCODEEDITORINTERFACE_H

#include "codeeditor.h"
#include "imgviewer.h"

#include <QFrame>
#include <QStackedWidget>
#include <QTabBar>

namespace Ui {
    class TabbedDocumentInterface;
}

class TabbedDocumentInterface : public QFrame
{
    Q_OBJECT

public:
    explicit TabbedDocumentInterface(QWidget *parent = nullptr);
    ~TabbedDocumentInterface();

    void openFile(const QString &filepath, bool reload = false);

    int getCurIndex() const;
    void setCurIndex(int i);

    CodeFile *getCurFile();
    QString getCurFilePath();
    QTextDocument *getCurDoc();
    QVector<CodeFile> *getFiles();
    CodeEditor *getCodeEditor() const;
    ImgViewer *getImgViewer() const;
    QTabBar *getTabBar() const;
    QStackedWidget *getStackedWidget();

    int count() const;
    bool hasNoFile() const;
    void clear();
    bool hasUnsavedChanges() const;

public /*slots*/ :
    void onOpenFile(const QString &filepath);
    bool saveCurFile();
    bool saveCurFile(const QString path);
    bool saveAllFile();
    void onFileRenamed(const QString &path, const QString &oldName,
                       const QString &newName);
    void onGameVersionChanged(const QString &ver);
    void undo();
    void redo();
    void selectAll();
    void cut();
    void copy();
    void paste();

signals:
    void curFileChanged(const QString &path);
    void curModificationChanged(bool changed);

protected:
    void changeEvent(QEvent *event) override;

private /*slots*/ :
    void onModificationChanged(bool changed);
    void onTabChanged(int index);
    void onTabMoved(int from, int to);
    void onCloseFile(int index);
    void onSwitchNextFile();
    void onSwitchPrevFile();
    void onCurTextChanged();
    void onCurTextChangingDone();
    void onCurFileChanged(const QString &path);

private:
    Ui::TabbedDocumentInterface *ui;

    QVector<CodeFile> files;
    int prevIndex                 = -1;
    QTimer *textChangedTimer      = nullptr;
    QTextDocument *lastRemovedDoc = nullptr;
    bool tabMovedOrRemoved        = false;

    CodeFile readFile(const QString &path);
    void addCodeFile(const CodeFile &file);
    bool saveFile(int index, const QString &filepath);
    void updateTabTitle(int index, bool changed = false);

    bool maybeSave(int index);
    void retranslate();
    void printPanOffsets();
    QTextDocument *getDocAt(int index) const;
    void saveFileData(int index);
};

#endif /* TABBEDCODEEDITORINTERFACE_H */
