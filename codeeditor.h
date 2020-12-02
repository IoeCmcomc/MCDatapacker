#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
/*#include <QTextEdit> */
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
    QWidget *lineNumberArea;
    CodeFile::FileType curFileType;
    QString filepath;
    QString prevFilepath;
    QStringList keyModifiers;
    JsonHighlighter *jsonHighlighter;
    MCfunctionHighlighter *mcfunctionHighlighter;
    QTextCursor lastTextCursor;
    QTextCursor mouseTextCursor;
    QTextCursor lastMouseTextCursor;

    void highlightCurrLineSelection();
    void followCurrentNamespacedID();
};


#endif /* CODEEDITOR_H */
