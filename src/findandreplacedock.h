#ifndef FINDANDREPLACEDOCK_H
#define FINDANDREPLACEDOCK_H

#include <QDockWidget>

QT_BEGIN_NAMESPACE
class QDirIterator;
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

signals:
    void findCurFileRequested(const QString &query, Options options);
    void replaceCurFileRequested(const QString &text);
    void replaceAllCurFileRequested(const QString &query, const QString &text,
                                    Options options);
    void openFileRequested(const QString &filePath);
    void resetCursorRequested();

public slots:
    void onFindCurFileCompleted(const bool found);

protected:
    void changeEvent(QEvent *e);

private slots:
    void onQueryChanged(const QString &text);
    void onFoldBtnClicked(const bool checked);
    void onFindBtnClicked();
    void onReplaceBtnClicked();
    void onReplaceAll();

private:
    Ui::FindAndReplaceDock *ui;
    QDirIterator *m_dirIter = nullptr;
    bool m_continue         = false;
    bool m_continueFile     = false;

    bool findInFile(const QString &path);
    void showMatchInEditor(const QString &path, const QString &query);
};

#endif // FINDANDREPLACEDOCK_H
