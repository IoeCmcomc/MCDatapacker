#ifndef TABBEDCODEEDITORINTERFACE_H
#define TABBEDCODEEDITORINTERFACE_H

#include "codefile.h"

#include <QFrame>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QTabBar;
QT_END_NAMESPACE

class MainWindow;
class CodeEditor;
class ImgViewer;

namespace Ui {
    class TabbedDocumentInterface;
}

class TabbedDocumentInterface : public QFrame {
    Q_OBJECT

public:
    enum class ActionType {
        ZoomIn,
        ZoomOut,
    };

    explicit TabbedDocumentInterface(QWidget *parent = nullptr);
    ~TabbedDocumentInterface();

    void openFile(const QString &filepath,
                  const QString &realPath = {},
                  bool reload             = false);

    int getCurIndex() const;
    void setCurIndex(int i);

    CodeFile * getCurFile();
    QString getCurFilePath();
    QTextDocument * getCurDoc();
    QVector<CodeFile> &getFiles();
    CodeEditor * getCodeEditor() const;
    ImgViewer * getImgViewer() const;

    int count() const;
    bool hasNoFile() const;
    void clear();
    bool hasUnsavedChanges() const;

    static QString readTextFile(QWidget *parent,
                                const QString &path, bool &ok);

public /*slots*/ :
    void onOpenFile(const QString &filepath);
    void onOpenAliasedFile(const QString &filepath, const QString &realPath);
    void onOpenFileWithLine(const QString &filepath, const int lineNo);
    void onOpenFileWithSelection(const QString &filepath, const int lineNo,
                                 const int colNo, const int selLength = 0);
    bool saveCurFile();
    bool saveCurFile(const QString &path);
    bool saveAllFile();
    void onFileRenamed(const QString &path, const QString &oldName,
                       const QString &newName);
    template<typename Func, typename ...Args>
    void invokeCodeEditor(Func method, Args ... args) {
        if (auto *editor = getCodeEditor()) {
            (editor->*method)(std::forward<Args>(args)...);
        }
    };
    void invokeActionType(ActionType act);
    void setPackOpened(const bool value);
    void updateRecentPacks(const QVector<QAction *> &actions, const int size);

signals:
    void curFileChanged(const QString &path);
    void curModificationChanged(bool changed);
    void updateStatusBarRequest(QWidget *widget);
    void showMessageRequest(const QString &msg, int timeout);
    void updateEditMenuRequest();
    void settingsChanged();
    void findCompleted(const bool found);

protected:
    void changeEvent(QEvent *event) override;

private /*slots*/ :
    void onModificationChanged(bool changed);
    void onTabChanged(int index);
    void onTabMoved(int from, int to);
    void onCloseFile(int index, const bool force = false);
    void onSwitchNextFile();
    void onSwitchPrevFile();

private:
    Ui::TabbedDocumentInterface *ui;

    QVector<CodeFile> files;
    bool m_packOpened = false;

    QString readTextFile(const QString &path, bool &ok);
    void addFile(const QString &path, const QString &realPath = {});
    bool saveFile(int index, const QString &filepath);
    void updateTabTitle(int index, bool changed = false);

    bool maybeSave(int index);
    void retranslate();
    QTextDocument * getDocAt(int index) const;
    QTabBar * getTabBar() const;
};

#endif /* TABBEDCODEEDITORINTERFACE_H */
