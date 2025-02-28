#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QPointer>
#include <QSettings>

#include "codefile.h"
#include "parsers/parser.h"
#include "findandreplacedock.h"

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
static_assert(qIsRelocatable<ProblemInfo>() == false);
Q_DECLARE_TYPEINFO(ProblemInfo, Q_RELOCATABLE_TYPE);

static_assert(qIsRelocatable<QTextEdit::ExtraSelection>() == false);
Q_DECLARE_TYPEINFO(QTextEdit::ExtraSelection,
                   Q_RELOCATABLE_TYPE);

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
    void findCompleted(const bool found);

public: // Slots
    void openFindDialog();
    void openReplaceDialog();
    bool find(const QString &text, FindAndReplaceDock::Options options);
    void replaceSelection(const QString &text);
    void replaceAllWith(const QString &query, const QString &text,
                        FindAndReplaceDock::Options options);
    void resetTextCursor();
    void toggleComment();
    void copyLineUp();
    void copyLineDown();
    void moveLineUp();
    void moveLineDown();
    void selectCurrentLine();

protected:
    friend class LineNumberArea;
    friend class ProblemArea;
    friend class StripedScrollBar;

    void resizeEvent(QResizeEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
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
    void onUndoAvailable(bool value);
    void onRedoAvailable(bool value);
    void insertCompletion(const QString &completion);
    void onTextChanged();

private:
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
    int m_fontSize                  = 13;
    int m_tabSize                   = 4;
    CodeFile::FileType m_fileType   = CodeFile::Text;
    bool canUndo                    = false;
    bool canRedo                    = false;
    bool m_insertTabAsSpaces        = true;
    bool m_needCompleting           = false;
    bool m_doHighlightSameSelection = true;

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
    void highlightOccurrences(const QString &text,
                              FindAndReplaceDock::Options options,
                              QTextCharFormat format);
    void highlightSameSelectedText();

    QString textUnderCursor() const;
    void handleKeyPressEvent(QKeyEvent *e);
    void indentOnNewLine(QKeyEvent *e);
    void initCompleter();
    bool findCursor(QTextCursor &cursor, const QString &text,
                    FindAndReplaceDock::Options options);
    void replaceSelectedCursor(QTextCursor &cursor, const QString &text);
    static void startOfWordExtended(QTextCursor &tc);
    static QString textUnderCursorExtended(QTextCursor tc);
    static void selectEnclosingLines(QTextCursor &cursor);

    void startCompletion(const QString &completionPrefix);

    void applyHighlighterPalette();
    void patchPalette();
};


#endif /* CODEEDITOR_H */
