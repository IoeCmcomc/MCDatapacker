#include "codeeditor.h"

#include "linenumberarea.h"
#include "jsonhighlighter.h"
#include "datapacktreeview.h"
#include "mainwindow.h"
#include "globalhelpers.h"
#include "QFindDialogs/src/finddialog.h"
#include "QFindDialogs/src/findreplacedialog.h"

#include <QDebug>
#include <QPainter>
#include <QTextBlock>
#include <QTextCursor>
#include <QFileInfo>
#include <QStringLiteral>
#include <QSplitter>
#include <QApplication>
#include <QMimeData>
#include <QJsonDocument>
#include <QFont>
#include <QShortcut>
#include <QToolTip>
#include <QTextDocumentFragment>


CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {
    lineNumberArea = new LineNumberArea(this);
    setAttribute(Qt::WA_Hover);

    connect(this, &CodeEditor::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest,
            this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged,
            this, &CodeEditor::onCursorPositionChanged);

    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this),
            &QShortcut::activated, this, &CodeEditor::openFindDialog);
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_H), this),
            &QShortcut::activated, this, &CodeEditor::openReplaceDialog);
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Slash), this),
            &QShortcut::activated, this, &CodeEditor::toggleComment);

    readPrefSettings();

    bracketSeclectFmt.setFontWeight(QFont::Bold);
    bracketSeclectFmt.setForeground(Qt::red);
    /*bracketSeclectFmt.setBackground(QColor("#BB00FF00")); */
    /*bracketSeclectFmt.setBackground(Qt::white); */
    bracketSeclectFmt.setBackground(QColor(102, 227, 102, 170));

    /*
       errorHighlightRule.setUnderlineStyle(
          QTextCharFormat::SpellCheckUnderline);
       errorHighlightRule.setUnderlineColor(Qt::red);
     */
    errorHighlightRule.setBackground(QColor(255, 127, 127, 63));
    errorHighlightRule.setProperty(QTextFormat::FullWidthSelection,
                                   true);

    updateLineNumberAreaWidth(0);
    onCursorPositionChanged();
}

void CodeEditor::readPrefSettings() {
    QSettings settings;

    settings.beginGroup("editor");

    if (settings.contains("textFont")) {
        monoFont = qvariant_cast<QFont>(settings.value("textFont"));
    } else {
        monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    }
    monoFont.setPointSize(settings.value("textSize", 13).toInt());

    setFont(monoFont);
    setLineWrapMode(settings.value("wrap", false).toBool()
                        ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);

    settings.endGroup();
}

void CodeEditor::wheelEvent(QWheelEvent *e) {
    if (e->modifiers().testFlag(Qt::ControlModifier)) {
        int delta = e->angleDelta().y() / 120;
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
        auto dirpath = QDir::currentPath();
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
    auto     dirpath      = QDir::currentPath();
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

bool CodeEditor::event(QEvent *event) {
    if (event->type() == QEvent::ToolTip) {
        /*QToolTip::hideText(); */
        qDebug() << "QEvent::ToolTip";

        auto *helpEvent = static_cast<QHelpEvent*>(event);
        auto  globalPos = mapToGlobal(helpEvent->pos());
        auto  pos       = helpEvent->pos();
        bool  done      = false;

        pos.rx() -= viewportMargins().left();
        pos.ry() -= viewportMargins().top();
        auto cursor    = cursorForPosition(pos);
        auto block     = cursor.block();
        int  cursorPos = cursor.positionInBlock();

        for (const auto &selection: qAsConst(problemExtraSelections)) {
            auto selectionCursor = selection.cursor;
            if (selection.cursor.block().contains(cursor.position())) {
                QToolTip::showText(globalPos,
                                   selection.format.toolTip());
                done = true;
                break;
            }
        }
        if (done)
            return true;

        const auto formats = block.layout()->formats();
        for (const auto &format: qAsConst(formats)) {
            int formatStart = format.start;
            if ((formatStart <= cursorPos)
                && (cursorPos <= formatStart + format.length)
                && !format.format.toolTip().isEmpty()) {
                QToolTip::showText(globalPos, format.format.toolTip());
                done = true;
                break;
            }
        }
        if (done)
            return true;

        cursor.select(QTextCursor::WordUnderCursor);
        if (!cursor.selectedText().isEmpty()) {
            QToolTip::showText(globalPos,
                               QString("%1 %2").arg(cursor.selectedText(),
                                                    QString::number(cursor.
                                                                    selectedText()
                                                                    .length())));
        } else {
            QToolTip::hideText();
        }
        qDebug() << QToolTip::text() << QToolTip::isVisible();

        return true;
    } else {
        return QPlainTextEdit::event(event);
    }
}

void CodeEditor::onCursorPositionChanged() {
    /*qDebug() << "CodeEditor::onCursorPositionChanged"; */
    setExtraSelections({});
    /*highlightCurrentLine(); */
    matchParentheses();
    problemSelectionStartIndex = extraSelections().size() - 1;
    if (!problemExtraSelections.isEmpty()) {
        setExtraSelections(extraSelections() << problemExtraSelections);
    }
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
    filepath = path;

    document()->setDefaultFont(monoFont);
    setFont(monoFont);
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

void CodeEditor::openFindDialog() {
    auto *fdialog = new FindDialog(this);

    fdialog->setEditor(this);
    fdialog->show();
}

void CodeEditor::openReplaceDialog() {
    auto *frdialog = new FindReplaceDialog(this);

    frdialog->setEditor(this);
    frdialog->show();
}

void CodeEditor::toggleComment() {
    if (!curHighlighter ||
        curHighlighter->singleCommentHighlightRules.keys().isEmpty())
        return;

    auto       txtCursor    = textCursor();
    const auto posBlock     = txtCursor.block();
    const int  posInBlock   = txtCursor.positionInBlock();
    auto       anchorCursor = txtCursor;

    anchorCursor.setPosition(txtCursor.anchor());
    const auto anchorBlock      = anchorCursor.block();
    const int  anchorPosInBlock = anchorCursor.positionInBlock();

    txtCursor.beginEditBlock();
    const bool fromTopDown = txtCursor.position() > txtCursor.anchor();;
    int        count       = abs(
        posBlock.blockNumber() - anchorBlock.blockNumber()) +
                             1;
    bool skippedLastLine = false;
    if (txtCursor.hasSelection() &&
        ((fromTopDown) ? txtCursor.atBlockStart() : anchorCursor.atBlockStart()))
    {
        if (fromTopDown)
            txtCursor.movePosition(QTextCursor::PreviousBlock);
        count--;
        skippedLastLine = true;
    }
    txtCursor.clearSelection();

    QSettings  settings;
    const bool fillMode = !settings.value("editor/toggleComments",
                                          false).toBool();
    bool        anchorShouldComment = false;
    int         anchorCharAdded     = 0;
    int         posCharAdded        = 0;
    const QChar commentChar         =
        curHighlighter->singleCommentHighlightRules.keys()[0];

    for (int i = 0; i < count; i++) {
        txtCursor.movePosition(QTextCursor::StartOfLine);
        const QString curLine    = txtCursor.block().text();
        const bool    hasComment = curLine.startsWith(commentChar);
        if (i == 0) {
            anchorShouldComment = !hasComment;
        }
        int  charAdded     = 0;
        bool shouldComment = (fillMode) ? anchorShouldComment : !hasComment;
        if (shouldComment) {
            if (!hasComment) {
                txtCursor.insertText(commentChar);
                charAdded = 1;
            }
        } else if (!curLine.isEmpty()) {
            if (hasComment) {
                txtCursor.deleteChar();
                charAdded = -1;
            }
        }
        if (i == 0) {
            anchorCharAdded = charAdded;
        }
        if (i < count - 1) {
            txtCursor.movePosition(
                (fromTopDown) ? QTextCursor::PreviousBlock : QTextCursor::NextBlock);
        } else if (i == count - 1) {
            posCharAdded = charAdded;
        }
    }

    txtCursor.setPosition(anchorBlock.position() + anchorPosInBlock +
                          anchorCharAdded * !(!fromTopDown && skippedLastLine));
    txtCursor.setPosition(posBlock.position() + posInBlock +
                          posCharAdded * !(fromTopDown && skippedLastLine),
                          QTextCursor::KeepAnchor);
    txtCursor.endEditBlock();
    setTextCursor(txtCursor);
}

void CodeEditor::setCurHighlighter(Highlighter *value) {
    curHighlighter = value;
}

void CodeEditor::displayErrors() {
    qDebug() << "CodeEditor::displayErrors";
    if (!isReadOnly()) {
        auto selections = extraSelections().mid(0,
                                                problemSelectionStartIndex + 1);
        selections += problemExtraSelections;

        setExtraSelections(selections);
    }
}

void CodeEditor::updateErrorSelections() {
    qDebug() << "CodeEditor::updateErrorSelections";
    if (!isReadOnly()) {
        if (!document() || !curHighlighter)
            return;

        problemExtraSelections.clear();

        for (auto it = document()->firstBlock();
             it != document()->end(); it = it.next()) {
            if (TextBlockData *data =
                    dynamic_cast<TextBlockData *>(it.userData())) {
                if (std::optional<ProblemInfo> problem =
                        data->problem(); problem) {
                    QTextEdit::ExtraSelection selection;
                    selection.cursor = textCursor();
                    selection.cursor.setPosition(problem->start);
                    /*selection.cursor.select(QTextCursor::LineUnderCursor); */
                    selection.cursor.clearSelection();
                    selection.format = errorHighlightRule;
                    selection.format.setToolTip(problem->message);
                    problemExtraSelections << selection;
                }
/*
              qDebug() << "Line" << it.blockNumber()
                       << ", has error:"
                       << (data->problem() != std::nullopt);
 */
            }
        }
    }
    displayErrors();
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

