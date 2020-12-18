#include "codeeditor.h"

#include "linenumberarea.h"
#include "jsonhighlighter.h"
#include "datapacktreeview.h"
#include "mainwindow.h"
#include "globalhelpers.h"

#include <QDebug>
#include <QPainter>
#include <QTextBlock>
#include <QTextCursor>
#include <QFileInfo>
#include <QStringLiteral>
#include <QSplitter>
#include <QApplication>
#include <QMimeData>
#include <QDir>
#include <QJsonDocument>
#include <QFont>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {
    lineNumberArea        = new LineNumberArea(this);
    jsonHighlighter       = new JsonHighlighter(this);
    mcfunctionHighlighter = new McfunctionHighlighter(this);

    connect(this, &CodeEditor::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest,
            this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged,
            this, &CodeEditor::onCursorPositionChanged);

    monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monoFont.setPointSize(13);

    setFont(monoFont);

    bracketSeclectFmt.setFontWeight(QFont::Bold);
    bracketSeclectFmt.setForeground(Qt::red);
    /*bracketSeclectFmt.setBackground(QColor("#BB00FF00")); */
    /*bracketSeclectFmt.setBackground(Qt::white); */
    bracketSeclectFmt.setBackground(QColor(102, 227, 102, 170));

    updateLineNumberAreaWidth(0);
    onCursorPositionChanged();
}

void CodeEditor::wheelEvent(QWheelEvent *e) {
    if (e->modifiers().testFlag(Qt::ControlModifier)) {
        int delta = e->delta() / 120;
        monoFont.setPointSize(monoFont.pointSize() + delta);
        this->setFont(monoFont);
    }
    QPlainTextEdit::wheelEvent(e);
}

void CodeEditor::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                      lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::mouseMoveEvent(QMouseEvent *e) {
    QPlainTextEdit::mouseMoveEvent(e);

/*
      / *if(e->buttons() == Qt::LeftButton) {} * /
      QTextCursor cursor = cursorForPosition(e->pos());
      this->mouseTextCursor = cursor;
 */

/*
      if (cursor.blockNumber() != this->lastMouseTextCursor.blockNumber())
          this->mcfunctionHighlighter->rehighlightBlock(
              this->document()->findBlockByNumber(this->lastMouseTextCursor.
                                                  blockNumber()));
      this->mcfunctionHighlighter->rehighlightBlock(
          this->document()->findBlockByNumber(cursor.blockNumber()));
 */

/*    this->lastMouseTextCursor = cursorForPosition(e->pos()); */
}

void CodeEditor::mousePressEvent(QMouseEvent *e) {
/*
      qDebug() << "Mouse press event" << QGuiApplication::keyboardModifiers() <<
          e->modifiers();
 */

    QPlainTextEdit::mousePressEvent(e);

    if (QGuiApplication::keyboardModifiers() & Qt::ControlModifier
        || e->modifiers() & Qt::ControlModifier) {
        followNamespacedId(e);
    }
}

void CodeEditor::dropEvent(QDropEvent *e) {
    QString nspacedID;

    if (e->mimeData()->hasFormat("text/uri-list")) {
        auto path    = e->mimeData()->urls().at(0).toLocalFile();
        auto dirpath = MainWindow::getCurDir();
        nspacedID = Glhp::toNamespacedID(dirpath, path);
    } else if (e->mimeData()->hasText()) {
        nspacedID = e->mimeData()->text();
        if (!nspacedID.contains(':'))
            nspacedID = QStringLiteral("minecraft:") + nspacedID;
    }
    if (!nspacedID.isEmpty()) {
        QTextCursor cursor = cursorForPosition(e->pos());
        cursor.beginEditBlock();
        cursor.insertText(nspacedID);
        cursor.endEditBlock();
        setTextCursor(cursor);

        auto *mimeData = new QMimeData();
        mimeData->setText("");
        auto *dummyEvent
            = new QDropEvent(e->posF(), e->possibleActions(),
                             mimeData, e->mouseButtons(),
                             e->keyboardModifiers());

        QPlainTextEdit::dropEvent(dummyEvent);
        delete mimeData;
        return;
    }
    QPlainTextEdit::dropEvent(e);
}

void CodeEditor::contextMenuEvent(QContextMenuEvent *e) {
    QMenu *menu = createStandardContextMenu(e->pos());

    QAction *sep1Action = new QAction("More", menu);

    sep1Action->setSeparator(true);
    menu->addAction(sep1Action);

    QAction *formatAction = new QAction(tr("Format"), menu);
    auto     dirpath      = MainWindow::getCurDir();
    auto     fileType     = Glhp::pathToFileType(dirpath, filepath);

    formatAction->setDisabled(!((CodeFile::JsonText <= fileType) &&
                                (fileType <= CodeFile::ItemTag)));
    if (formatAction->isEnabled()) {
        connect(formatAction, &QAction::triggered, [ = ]() {
            QJsonDocument doc = QJsonDocument::fromJson(toPlainText().toUtf8());
            if (doc.isNull()) return;

            QTextCursor cursor = cursorForPosition(e->pos());
            cursor.beginEditBlock();
            cursor.select(QTextCursor::Document);
            cursor.insertText(doc.toJson(QJsonDocument::Indented));
            cursor.endEditBlock();
            setTextCursor(cursor);
        });
    }
    menu->addAction(formatAction);

    /*... */
    menu->exec(e->globalPos());
    delete menu;
}

void CodeEditor::onCursorPositionChanged() {
    setExtraSelections({});
    highlightCurrentLine();
    matchParentheses();
}


void CodeEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> selections = extraSelections();

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(237, 236, 223, 127);

        selection.format = QTextCharFormat();
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();

        selections.append(selection);
    }
    setExtraSelections(selections);
}

void CodeEditor::setFilePath(const QString &path) {
    filepath    = path;
    curFileType = Glhp::pathToFileType(MainWindow::getCurDir(), path);

    document()->setDefaultFont(monoFont);
    setFont(monoFont);

    if (curFileType == CodeFile::Function) {
        jsonHighlighter->setDocument(nullptr);
        mcfunctionHighlighter->setDocument(document());
        curHighlighter = mcfunctionHighlighter;
    } else if (curFileType >= CodeFile::JsonText) {
        mcfunctionHighlighter->setDocument(nullptr);
        jsonHighlighter->setDocument(document());
        curHighlighter = jsonHighlighter;
    } else {
        jsonHighlighter->setDocument(nullptr);
        mcfunctionHighlighter->setDocument(nullptr);
        curHighlighter = nullptr;
    }
}

int CodeEditor::lineNumberAreaWidth() {
    int digits = 1;
    int max    = qMax(1, blockCount());

    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(),
                               rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(lineNumberArea);

    painter.fillRect(event->rect(), QColor(210, 210, 210));
    /*
       painter.setPen(QColor(240, 240, 240));
       painter.drawLine(event->rect().topRight(), event->rect().bottomRight());
     */
    QTextBlock block       = firstVisibleBlock();
    int        blockNumber = block.blockNumber();
    int        top         =
        qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            if (blockNumber == this->textCursor().blockNumber()) {
                painter.setPen(QColor(40, 40, 40));
            } else {
                painter.setPen(Qt::darkGray);
            }
            painter.drawText(0,
                             top,
                             lineNumberArea->width(),
                             fontMetrics().height(),
                             Qt::AlignRight,
                             number);
        }

        block  = block.next();
        top    = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::matchParentheses() {
    /*bool match = false; */

    TextBlockData *data =
        dynamic_cast<TextBlockData *>(textCursor().block().userData());

    if (!data || !curHighlighter)
        return;

    QVector<BracketInfo*> infos = data->brackets();

    int pos = textCursor().block().position();

    for (int i = 0; i < infos.size(); ++i) {
        const BracketInfo *info = infos.at(i);

        int curPos = textCursor().position() -
                     textCursor().block().position();
        /* info->position == curPos:
         *     the text cursor is on the left of the character
         * info->position == curPos - 1
         *     the text cursor is on the right of the character */
        bool isOnTheLeftOfChar = info->position == curPos;

        if (isOnTheLeftOfChar || (info->position == curPos - 1)) {
            for (const auto &pair: curHighlighter->bracketPairs) {
                if (info->character == pair.left) {
                    if (matchLeftBracket(textCursor().block(), i + 1,
                                         pair.left, pair.right,
                                         0, isOnTheLeftOfChar)) {
                        createBracketSelection(pos + info->position,
                                               isOnTheLeftOfChar);
                    }
                } else if (info->character == pair.right) {
                    if (matchRightBracket(textCursor().block(), i - 1,
                                          pair.right, pair.left,
                                          0, !isOnTheLeftOfChar)) {
                        createBracketSelection(pos + info->position,
                                               !isOnTheLeftOfChar);
                    }
                }
            }
        }
    }
}

bool CodeEditor::matchLeftBracket(QTextBlock currentBlock,
                                  int i, char chr, char corresponder,
                                  int numLeftParentheses, bool isPrimary) {
    auto *data =
        dynamic_cast<TextBlockData *>(currentBlock.userData());
    QVector<BracketInfo *> infos = data->brackets();


/*
      qDebug() << "matchLeftBracket" << chr << corresponder << i <<
          numLeftParentheses << infos.count();
 */


    int docPos = currentBlock.position();

    for (; i < infos.size(); ++i) {
        BracketInfo *info = infos.at(i);

        if (info->character == chr) {
            ++numLeftParentheses;
            continue;
        }

        if (info->character == corresponder) {
            if (numLeftParentheses == 0) {
                createBracketSelection(docPos + info->position, isPrimary);
                return true;
            } else {
                --numLeftParentheses;
            }
        }
    }

    currentBlock = currentBlock.next();
    if (currentBlock.isValid())
        return matchLeftBracket(currentBlock, 0, chr, corresponder,
                                numLeftParentheses, isPrimary);

    return false;
}

bool CodeEditor::matchRightBracket(QTextBlock currentBlock,
                                   int i, char chr, char corresponder,
                                   int numRightParentheses, bool isPrimary) {
    auto *data =
        dynamic_cast<TextBlockData *>(currentBlock.userData());
    QVector<BracketInfo *> infos = data->brackets();

    if (i == -2)
        i = infos.count() - 1;

/*
      qDebug() << "matchRightBracket" << chr << corresponder << i <<
          numRightParentheses << infos.count();
 */

    int docPos = currentBlock.position();

    for (; i > -1 && !infos.isEmpty(); --i) {
        BracketInfo *info = infos.at(i);
        if (info->character == chr) {
            ++numRightParentheses;
            continue;
        }

        if (info->character == corresponder) {
            if (numRightParentheses == 0) {
                createBracketSelection(docPos + info->position, isPrimary);
                return true;
            } else {
                --numRightParentheses;
            }
        }
    }

    currentBlock = currentBlock.previous();
    if (currentBlock.isValid())
        return matchRightBracket(currentBlock, -2, chr, corresponder,
                                 numRightParentheses, isPrimary);

    return false;
}

void CodeEditor::createBracketSelection(int pos, bool isPrimary) {
    /*qDebug() << "createBracketSelection" << pos; */
    QList<QTextEdit::ExtraSelection> selections = extraSelections();

    QTextEdit::ExtraSelection selection;

    selection.format = bracketSeclectFmt;
    if (!isPrimary)
        selection.format.setBackground(
            selection.format.background().color().lighter());

    QTextCursor cursor = textCursor();
    cursor.setPosition(pos);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    selection.cursor = cursor;

    selections.append(selection);

    setExtraSelections(selections);
}

void CodeEditor::followNamespacedId(const QMouseEvent *event) {
    TextBlockData *data =
        dynamic_cast<TextBlockData *>(textCursor().block().userData());

    if (!data || !curHighlighter)
        return;

    QVector<NamespacedIdInfo *> infos = data->namespacedIds();

    /*qDebug() << "followNamespacedId" << infos.count(); */

    auto cursor         = cursorForPosition(event->pos());
    auto cursorBlockPos = cursor.positionInBlock();

    for (const auto info: infos) {
        if (cursorBlockPos >= info->start &&
            cursorBlockPos <= (info->start + info->length)) {
/*
              qDebug() << cursor.position() <<
                  (cursor.position() - cursorBlockPos + info->start) <<
                  cursor.positionInBlock() <<
                  info->start << (info->start + info->length);
 */
            emit openFile(info->link);
            break;
        }
    }
}

