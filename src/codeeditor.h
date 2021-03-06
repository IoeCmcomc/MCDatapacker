#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

#include "codefile.h"

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
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    bool event(QEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void onCursorPositionChanged();
    void updateLineNumberArea(const QRect &rect, int dy);
    void openFindDialog();
    void openReplaceDialog();

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
