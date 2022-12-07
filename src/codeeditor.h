#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QPointer>
#include <QSettings>
#include <QStringList>

#include "codefile.h"

QT_BEGIN_NAMESPACE
class QCompleter;
QT_END_NAMESPACE

class CodeGutter;
class Highlighter;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    void setFileType(CodeFile::FileType type);

    Highlighter * getCurHighlighter() const;
    void setCurHighlighter(Highlighter *value);

    void displayErrors();
    void updateErrorSelections();

    void readPrefSettings();

    bool getCanUndo() const;
    bool getCanRedo() const;
    int problemCount() const;

    void setCompleter(QCompleter *c);
    QCompleter * completer() const;

signals:
    void openFileRequest(const QString &filepath);
    void updateStatusBarRequest(CodeEditor *editor);
    void showMessageRequest(const QString &msg, int timeout);

protected:
    friend class LineNumberArea;
    friend class ProblemArea;

    void resizeEvent(QResizeEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    bool event(QEvent *event) override;
    void focusInEvent(QFocusEvent *e) override;

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

private:
    QTextCharFormat bracketSeclectFmt;
    QTextCharFormat errorHighlightRule;
    QTextCharFormat warningHighlightRule;
    QStringList minecraftCompletionInfo;
    CodeGutter *m_gutter;
    QCompleter *m_completer     = nullptr;
    Highlighter *curHighlighter = nullptr;
    QList<QTextEdit::ExtraSelection> problemExtraSelections;
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
};


#endif /* CODEEDITOR_H */
