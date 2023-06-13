#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QPointer>
#include <QSettings>
#include <QStringList>

#include "codefile.h"
#include "parsers/parser.h"

QT_BEGIN_NAMESPACE
class QCompleter;
QT_END_NAMESPACE

class CodeGutter;
class Highlighter;
class Parser;

struct ProblemInfo {
    enum class Type {
        Invalid,
        Warning,
        Error,
    };

    Type    type   = Type::Invalid;
    int     pos    = 0;
    int     length = 1;
    QString message;
};

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT

public:
    using Problems = QVector<ProblemInfo>;

    explicit CodeEditor(QWidget *parent = nullptr);

    void setFileType(CodeFile::FileType type);

    Highlighter * highlighter() const;
    void setHighlighter(Highlighter *value);

    void displayErrors();
    void updateErrorSelections();

    void readPrefSettings();

    bool getCanUndo() const;
    bool getCanRedo() const;
    int problemCount() const;

    void setCompleter(QCompleter *c);
    QCompleter * completer() const;

    void setParser(std::unique_ptr<Parser> newParser);
    Parser * parser() const;

    void goToLine(const int lineNo);

signals:
    void openFileRequest(const QString &filepath);
    void updateStatusBarRequest(CodeEditor *editor);
    void showMessageRequest(const QString &msg, int timeout);

protected:
    friend class LineNumberArea;
    friend class ProblemArea;
    friend class StripedScrollBar;

    void resizeEvent(QResizeEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;;
    void keyPressEvent(QKeyEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    bool event(QEvent *event) override;
    void focusInEvent(QFocusEvent *e) override;
    void changeEvent(QEvent *e) override;

private /*slots*/ :
    void updateGutterWidth(int newBlockCount);
    void onCursorPositionChanged();
    void updateGutter(const QRect &rect, int dy);
    void openFindDialog();
    void openReplaceDialog();
    void toggleComment();
    void onUndoAvailable(bool value);
    void onRedoAvailable(bool value);
    void insertCompletion(const QString &completion);
    void onTextChanged();

private:
    static QStringList minecraftCompletionInfo;

    QTextCharFormat bracketSeclectFmt;
    QTextCharFormat errorHighlightRule;
    QTextCharFormat warningHighlightRule;
    CodeGutter *m_gutter;
    QCompleter *m_completer          = nullptr;
    Highlighter *m_highlighter       = nullptr;
    std::unique_ptr<Parser> m_parser = nullptr;
    QList<QTextEdit::ExtraSelection> problemExtraSelections;
    Problems m_problems;
    int problemSelectionStartIndex;
    int m_fontSize                = 13;
    int m_tabSize                 = 4;
    CodeFile::FileType m_fileType = CodeFile::Text;
    bool canUndo                  = false;
    bool canRedo                  = false;
    bool m_insertTabAsSpaces      = true;

    void highlightCurrentLine();
    void matchParentheses();
    bool matchLeftBracket(QTextBlock currentBlock,
                          int i, char chr, char corresponder,
                          int numLeftParentheses, bool isPrimary);
    bool matchRightBracket(QTextBlock currentBlock,
                           int i, char chr, char corresponder,
                           int numRightParentheses, bool isPrimary);
    void createBracketSelection(int pos, bool isPrimary);
    void followNamespacedId(const QMouseEvent *event);

    QString textUnderCursor() const;
    void handleKeyPressEvent(QKeyEvent *e);
    void indentOnNewLine(QKeyEvent *e);
    void initCompleter();
    void startOfWordExtended(QTextCursor &tc) const;
    QString textUnderCursorExtended(QTextCursor tc) const;
};


#endif /* CODEEDITOR_H */
