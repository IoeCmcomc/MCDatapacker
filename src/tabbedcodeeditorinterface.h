#ifndef TABBEDCODEEDITORINTERFACE_H
#define TABBEDCODEEDITORINTERFACE_H

#include "codeeditor.h"

#include <QFrame>
#include <QTimer>

namespace Ui {
    class TabbedCodeEditorInterface;
}

class TabbedCodeEditorInterface : public QFrame
{
    Q_OBJECT

public:
    explicit TabbedCodeEditorInterface(QWidget *parent = nullptr);
    ~TabbedCodeEditorInterface();

    void openFile(const QString &filepath, bool reload = false);

    int getCurIndex() const;
    void setCurIndex(int i);

    CodeFile *getCurFile();
    QString getCurFilePath();
    QTextDocument *getCurDoc();

    QVector<CodeFile> *getFiles();

    CodeEditor *getEditor() const;

    inline int count() const {
        return files.count();
    };
    inline bool hasNoFile() const {
        return count() == 0;
    };

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

signals:
    void curFileChanged(const QString &path);
    /*void curContentsChange(int position, int charsRemoved, int charsAdded); */
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
    Ui::TabbedCodeEditorInterface *ui;

    QVector<CodeFile> files;
    int prevIndex = -1;
    QTimer *textChangedTimer;
    bool tabMoved = false;

    CodeFile readFile(const QString &path);
    void addCodeFile(const CodeFile &file);
    bool saveFile(int index, const QString &filepath);
    void updateTabTitle(int index, bool changed = false);

    bool maybeSave(int index);
    void retranslate();
    void printPanOffsets();
};

#endif /* TABBEDCODEEDITORINTERFACE_H */
