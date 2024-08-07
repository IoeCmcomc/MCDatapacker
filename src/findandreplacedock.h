#ifndef FINDANDREPLACEDOCK_H
#define FINDANDREPLACEDOCK_H

#include <QDockWidget>
#include <QTextDocument>

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

protected:
    void changeEvent(QEvent *e);

private slots:
    void onFoldBtnClicked(const bool checked);
    void onFindBtnClicked();
    void onReplaceBtnClicked();
    void onReplaceAll();

private:
    Ui::FindAndReplaceDock *ui;
};

#endif // FINDANDREPLACEDOCK_H
