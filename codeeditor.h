#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QString>

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
    CurrentNamespacedID getCurrentNamespacedID();

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

private:
    QFont monoFont;
    QWidget *lineNumberArea;
    CodeFile::FileType curFileType;
    QString filepath;
    QString prevFilepath;
    QStringList keyModifiers;
    JsonHighlighter *jsonHighlighter;
    McfunctionHighlighter *mcfunctionHighlighter;
    QTextCursor lastTextCursor;
    QTextCursor mouseTextCursor;
    QTextCursor lastMouseTextCursor;

    void highlightCurrentLine();

    void followCurrentNamespacedID();

    bool matchLeftParenthesis(QTextBlock currentBlock,
                              int i,
                              int numLeftParentheses);
    bool matchRightParenthesis(QTextBlock currentBlock,
                               int i,
                               int numRightParentheses);
    void createParenthesisSelection(int pos);
    void matchParentheses();
};


#endif /* CODEEDITOR_H */
