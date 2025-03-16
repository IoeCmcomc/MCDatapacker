#ifndef FINDANDREPLACEDOCK_H
#define FINDANDREPLACEDOCK_H

#include <QDockWidget>
#include <QTextDocument>

QT_BEGIN_NAMESPACE
class QDirIterator;
class QTreeWidgetItem;
QT_END_NAMESPACE;

namespace Ui {
    class FindAndReplaceDock;
}

class FindAndReplaceDock : public QDockWidget
{
    Q_OBJECT

public:
    enum class Option {
        MatchCase      = 1,
        FindWholeWord  = 2,
        UseRegex       = 4,
        WarpAround     = 8,
        FindAndReplace = 256,
        FindInDatapack = 512,
    };
    Q_DECLARE_FLAGS(Options, Option)
    Q_FLAG(Options)

    explicit FindAndReplaceDock(QWidget *parent = nullptr);
    ~FindAndReplaceDock();

    Options options() const;
    void setOptions(Options options);

    void setQuery(const QString &text);

    void focusToInput();

    static bool findCursor(const QTextDocument *doc, QTextCursor &cursor,
                           const QString &text,
                           FindAndReplaceDock::Options options);

signals:
    void findCurFileRequested(const QString &query,
                              FindAndReplaceDock::Options options);
    void replaceCurFileRequested(const QString &text);
    void replaceAllCurFileRequested(const QString &query, const QString &text,
                                    FindAndReplaceDock::Options options);
    void openFileRequested(const QString &filePath,
                           const int lineNo    = 0,
                           const int colNo     = 0,
                           const int selLength = 1);
    void resetCursorRequested();

public slots:
    void onFindCurFileCompleted(const bool found);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);

private slots:
    void onQueryChanged(const QString &text);
    void onFoldBtnClicked(const bool checked);
    void onFindBtnClicked();
    void onFindAllClicked();
    void onReplaceBtnClicked();
    void onReplaceOnly();
    void onReplaceAll();
    void clearResults();
    void updateButtonStates();
    void onResultsTreeDoubleClicked(QTreeWidgetItem *item);

private:
    Ui::FindAndReplaceDock *ui;
    QDirIterator *m_dirIter  = nullptr;
    QTextDocument *m_textDoc = nullptr;
    int m_matchCount         = 0;
    bool m_continue          = false;
    bool m_continueFile      = false;

    bool findInFile(const QString &path);
    void showMatchInEditor(const QString &path, const QString &query);
    bool mayHasMatchInText(const QString &text, const QString &query,
                           Options options);
    static int countFilesInDir(const QString &dirPath);
    void replaceAllInPack();
};

#endif // FINDANDREPLACEDOCK_H
