#include "linenumberarea.h"

#include "codegutter.h"

#include <QPainter>

LineNumberArea::LineNumberArea(CodeGutter *parent) : QWidget(parent), m_gutter(
        parent) {
}

QSize LineNumberArea::minimumSizeHint() const {
    return QSize(sizeHint().width(), 100);
}

QSize LineNumberArea::sizeHint() const {
    int digits = 1;
    int max    = qMax(1, m_gutter->editor()->blockCount());

    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits + 6;
    return QSize(space, 300);
}

void LineNumberArea::mousePressEvent(QMouseEvent *e) {
    QWidget::mousePressEvent(e);
    auto *codeEditor = m_gutter->editor();
    txtCursor = codeEditor->cursorForPosition(e->pos());
    txtCursor.movePosition(QTextCursor::StartOfBlock);
    txtCursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
    codeEditor->setTextCursor(txtCursor);
}

void LineNumberArea::mouseMoveEvent(QMouseEvent *e) {
    QWidget::mouseMoveEvent(e);
    auto  *codeEditor = m_gutter->editor();
    auto &&cursor     = codeEditor->cursorForPosition(e->pos());
    cursor.movePosition(QTextCursor::NextBlock);
    txtCursor.setPosition(cursor.position(), QTextCursor::KeepAnchor);
    if (!txtCursor.hasSelection()) {
        txtCursor.movePosition(QTextCursor::NextBlock,
                               QTextCursor::KeepAnchor);
    }
    codeEditor->setTextCursor(txtCursor);
}

void LineNumberArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.fillRect(event->rect(), palette().midlight());

    auto        *editor         = m_gutter->editor();
    QTextBlock &&block          = editor->firstVisibleBlock();
    int          blockNumber    = block.blockNumber();
    const int    curBlockNumber = editor->textCursor().blockNumber();

    int top =
        qRound(editor->blockBoundingGeometry(block).translated(
                   editor->contentOffset()).top());
    int bottom = top + qRound(editor->blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString &&number = QString::number(blockNumber + 1);
            if (blockNumber == curBlockNumber) {
                painter.setPen(palette().shadow().color());
            } else {
                painter.setPen(palette().mid().color());
            }
            painter.drawText(0, top, width() - 3, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block  = block.next();
        top    = bottom;
        bottom = top + qRound(editor->blockBoundingRect(block).height());
        ++blockNumber;
    }
}
