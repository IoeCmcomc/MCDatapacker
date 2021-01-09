#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

#include "jsonhighlighter.h"
#include "mcfunctionhighlighter.h"
#include "codefile.h"

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);
    struct CurrentNamespacedID {
        int     startingIndex;
        int     blockNumber;
        QString string;
        QString link;
    };

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void setFilePath(const QString &path);

signals:
    void openFile(const QString &filepath);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void onCursorPositionChanged();
    void updateLineNumberArea(const QRect &rect, int dy);
    void openFindDialog();
    void openReplaceDialog();

private:
    QFont monoFont;
    QWidget *lineNumberArea;
    CodeFile::FileType curFileType = CodeFile::Text;
    QString filepath;
    JsonHighlighter *jsonHighlighter;
    McfunctionHighlighter *mcfunctionHighlighter;
    QTextCharFormat bracketSeclectFmt;
    Highlighter *curHighlighter;

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
