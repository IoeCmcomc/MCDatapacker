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

struct TextFileData {
    CodeFile               *parent      = nullptr;
    QPointer<QTextDocument> doc         = new QTextDocument();
    QTextCursor             textCursor  = QTextCursor(doc);
    Highlighter            *highlighter = nullptr;

    TextFileData(QTextDocument * doc, CodeFile * parent = nullptr);
    TextFileData()                     = default;
    TextFileData(const TextFileData &) = default;
    ~TextFileData()                    = default;
};

Q_DECLARE_METATYPE(TextFileData)

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);

    void setFilePath(const QString &path);
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
    QFont monoFont;
    QTextCharFormat bracketSeclectFmt;
    QTextCharFormat errorHighlightRule;
    QTextCharFormat warningHighlightRule;
    QSettings settings;
    QStringList minecraftCompletionInfo;
    CodeGutter *m_gutter;
    QCompleter *m_completer = nullptr;
    QString filepath;
    Highlighter *curHighlighter;
    QList<QTextEdit::ExtraSelection> problemExtraSelections;
    int problemSelectionStartIndex;
    CodeFile::FileType curFileType = CodeFile::Text;
    bool canUndo                   = false;
    bool canRedo                   = false;

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
};


#endif /* CODEEDITOR_H */
