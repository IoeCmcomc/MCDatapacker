#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include "codeeditor.h"

#include <QWidget>
#include <QPaintEvent>

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor) {
    }

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }
    void mousePressEvent(QMouseEvent *e) override {
        QWidget::mousePressEvent(e);
        txtCursor = codeEditor->cursorForPosition(e->pos());
        txtCursor.movePosition(QTextCursor::StartOfBlock);
        txtCursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
        codeEditor->setTextCursor(txtCursor);
    };
    void mouseMoveEvent(QMouseEvent *e) override {
        QWidget::mouseMoveEvent(e);
        auto cursor = codeEditor->cursorForPosition(e->pos());
        cursor.movePosition(QTextCursor::NextBlock);
        txtCursor.setPosition(cursor.position(), QTextCursor::KeepAnchor);
        if (!txtCursor.hasSelection()) {
            txtCursor.movePosition(QTextCursor::NextBlock,
                                   QTextCursor::KeepAnchor);
        }
        codeEditor->setTextCursor(txtCursor);
    };

private:
    CodeEditor *codeEditor;
    QTextCursor txtCursor;
};

#endif /* LINENUMBERAREA_H */
