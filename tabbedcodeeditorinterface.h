#ifndef TABBEDCODEEDITORINTERFACE_H
#define TABBEDCODEEDITORINTERFACE_H

#include "codeeditor.h"

#include <QFrame>

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

    CodeEditor *getEditor() const;

    inline int count() const {
        return files.count();
    };
    inline bool isNoFile() const {
        return count() == 0;
    };

    void clear();

    bool hasUnsavedChanges() const;

public slots:
    void onOpenFile(const QString &filepath);
    bool saveCurFile();
    bool saveCurFile(const QString path);
    bool saveAllFile();
    void onFileRenamed(const QString &path, const QString &oldName,
                       const QString &newName);

signals:
    void curFileChanged(const QString &path);
    /*void curContentsChange(int position, int charsRemoved, int charsAdded); */
    void curModificationChanged(bool changed);

private slots:
    void onModificationChanged(bool changed);
    void onTabChanged(int index);
    void onCloseFile(int index);

private:
    Ui::TabbedCodeEditorInterface *ui;

    QVector<CodeFile> files;
    int curIndex = -1;

    CodeFile readFile(const QString &path);
    void addCodeFile(const CodeFile &file);
    bool saveFile(int index, const QString &filepath);
    void updateTabTitle(int index, bool changed = false);
};

#endif /* TABBEDCODEEDITORINTERFACE_H */