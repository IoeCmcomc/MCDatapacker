#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QTextEdit>
#include <QString>

#include "jsonhighlighter.h"
#include "mcfunctionhighlighter.h"

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);
    struct CurrentNamespacedID {
        int startingIndex;
        int blockNumber;
        QString string;
        QString link;
    };
    QTextCursor lastTextCursor;
    QTextCursor mouseTextCursor;
    QTextCursor lastMouseTextCursor;

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void setFileName(QString fileName);
    CurrentNamespacedID getCurrentNamespacedID();

protected:
    void resizeEvent(QResizeEvent *event) override;
    //void keyPressEvent(QKeyEvent *event) override;
    //void focusInEvent(QFocusEvent *e) override;
    //void keyReleaseEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;


private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void onCursorPositionChanged();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
    QString fileName;
    QString prevFileName;
    QStringList keyModifiers;
    JsonHighlighter *jsonHighlighter;
    MCfunctionHighlighter *mcfunctionHighlighter;

    void highlightCurrLineSelection();
    void followCurrentNamespacedID();
};


#endif // CODEEDITOR_H
