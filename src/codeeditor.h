#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QSettings>

#include "codefile.h"

struct TextFileData {
    CodeFile      *parent      = nullptr;
    QTextDocument *doc         = new QTextDocument();
    QTextCursor    textCursor  = QTextCursor(doc);
    Highlighter   *highlighter = nullptr;

    TextFileData(QTextDocument *doc, CodeFile *parent = nullptr);

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

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void setFilePath(const QString &path);

    void setCurHighlighter(Highlighter *value);

    void displayErrors();
    void updateErrorSelections();

    void readPrefSettings();

signals:
    void openFile(const QString &filepath);

protected:
    void resizeEvent(QResizeEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    bool event(QEvent *event) override;

private /*slots*/ :
    void updateLineNumberAreaWidth(int newBlockCount);
    void onCursorPositionChanged();
    void updateLineNumberArea(const QRect &rect, int dy);
    void openFindDialog();
    void openReplaceDialog();
    void toggleComment();

private:
    QFont monoFont;
    QTextCharFormat bracketSeclectFmt;
    QTextCharFormat errorHighlightRule;
    QTextCharFormat warningHighlightRule;
    QWidget *lineNumberArea;
    CodeFile::FileType curFileType = CodeFile::Text;
    QString filepath;
    Highlighter *curHighlighter;
    QList<QTextEdit::ExtraSelection> problemExtraSelections;
    int problemSelectionStartIndex;
    QSettings settings;

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
};


#endif /* CODEEDITOR_H */
