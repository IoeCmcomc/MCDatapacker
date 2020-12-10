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
    bracketSeclectFmt.setBackground(QColor("#AA66E366"));

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
    /*qDebug() << "Mouse press event"; */
    if (QGuiApplication::keyboardModifiers() & Qt::ControlModifier
        && e->modifiers() & Qt::ControlModifier) {
        followCurrentNamespacedID();
    }
    QPlainTextEdit::mousePressEvent(e);
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

        QMimeData *mimeData = new QMimeData();
        mimeData->setText("");
        QDropEvent *dummyEvent
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

void CodeEditor::matchParentheses() {
    /*bool match = false; */

    TextBlockData *data =
        static_cast<TextBlockData *>(textCursor().block().userData());

    if (!data || !curHighlighter)
        return;

    QVector<BracketInfo *> infos = data->brackets();

    int  pos            = textCursor().block().position();
    auto highlightPairs = curHighlighter->bracketPairs;

    for (int i = 0; i < infos.size(); ++i) {
        BracketInfo *info = infos.at(i);

        int curPos = textCursor().position() -
                     textCursor().block().position();
        if ((info->position == curPos) || (info->position == curPos - 1)) {
            for (auto iter = highlightPairs.begin();
                 iter != highlightPairs.end(); ++iter) {
                if (info->character == iter->left) {
                    if (matchLeftBracket(textCursor().block(), i + 1,
                                         iter->left, iter->right,
                                         0)) {
                        createBracketSelection(pos + info->position);
                        break;
                    }
                } else if (info->character == iter->right) {
                    if (matchRightBracket(textCursor().block(), i - 1,
                                          iter->right, iter->left,
                                          0)) {
                        createBracketSelection(pos + info->position);
                        break;
                    }
                }
            }
        }
    }
}

bool CodeEditor::matchLeftBracket(QTextBlock currentBlock,
                                  int i, char chr, char corresponder,
                                  int numLeftParentheses) {
    TextBlockData *data =
        static_cast<TextBlockData *>(currentBlock.userData());
    QVector<BracketInfo *> infos = data->brackets();

    qDebug() << "matchLeftBracket" << chr << corresponder << i <<
        numLeftParentheses << infos.count();

    int docPos = currentBlock.position();

    for (; i < infos.size(); ++i) {
        BracketInfo *info = infos.at(i);

        if (info->character == chr) {
            ++numLeftParentheses;
            continue;
        }

        if (info->character == corresponder && numLeftParentheses == 0) {
            createBracketSelection(docPos + info->position);
            return true;
        } else
            --numLeftParentheses;
    }

    currentBlock = currentBlock.next();
    if (currentBlock.isValid())
        return matchLeftBracket(currentBlock, 0, chr, corresponder,
                                numLeftParentheses);

    return false;
}

bool CodeEditor::matchRightBracket(QTextBlock currentBlock,
                                   int i, char chr, char corresponder,
                                   int numRightParentheses) {
    TextBlockData *data =
        static_cast<TextBlockData *>(currentBlock.userData());
    QVector<BracketInfo *> infos = data->brackets();

    if (i == -2)
        i = infos.count() - 1;

    qDebug() << "matchRightBracket" << chr << corresponder << i <<
        numRightParentheses << infos.count();

    int docPos = currentBlock.position();

    for (; i > -1 && infos.size() > 0; --i) {
        BracketInfo *info = infos.at(i);
        if (info->character == chr) {
            ++numRightParentheses;
            continue;
        }
        if (info->character == corresponder && numRightParentheses == 0) {
            createBracketSelection(docPos + info->position);
            return true;
        } else
            --numRightParentheses;
    }

    currentBlock = currentBlock.previous();
    if (currentBlock.isValid())
        return matchRightBracket(currentBlock, -2, chr, corresponder,
                                 numRightParentheses);

    return false;
}

void CodeEditor::createBracketSelection(int pos) {
    QList<QTextEdit::ExtraSelection> selections = extraSelections();

    QTextEdit::ExtraSelection selection;

    selection.format = bracketSeclectFmt;

    QTextCursor cursor = textCursor();
    cursor.setPosition(pos);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    selection.cursor = cursor;

    selections.append(selection);

    setExtraSelections(selections);
}

CodeEditor::CurrentNamespacedID CodeEditor::getCurrentNamespacedID() {
    QTextCursor cursor = this->mouseTextCursor;
    /*
       qDebug() << "cursor pos: " << cursor.position();
       qDebug() << "cursor pos in block: " << cursor.positionInBlock();
       qDebug() << "cursor block number: " << cursor.blockNumber();
       qDebug() << "cursor column: " << cursor.columnNumber();
     */

    QTextBlock block = this->document()->findBlockByNumber(
        cursor.blockNumber());
    QString lineText = block.text();
    /*qDebug() << lineText; */
    int cursorRelPos = cursor.positionInBlock();

    QChar currChar = lineText[cursor.columnNumber()];

    if (QRegularExpression(QStringLiteral("[^0-9a-z-_.:/]")).match(currChar).
        hasMatch()) {
        --cursorRelPos;
    }

    bool isStartofString = false;
    while (!isStartofString && cursorRelPos >= 0) {
        currChar = lineText[cursorRelPos];
        /*qDebug() << "current character: " << currChar; */
        QRegularExpression regex =
            QRegularExpression(QStringLiteral("[#0-9a-z-_.:/]"));
        QRegularExpressionMatch match = regex.match(currChar);
        isStartofString = !match.hasMatch();
        /*qDebug() << "is start of string: " << isStartofString; */
        if (!isStartofString && cursorRelPos >= 0) --cursorRelPos;
    }
    /*qDebug() << "startIndex: " << cursorRelPos; */
    int                startIndex        = qMax(cursorRelPos + 1, 0);
    QString            str               = lineText.mid(startIndex);
    QRegularExpression namespacedIDRegex =
        QRegularExpression(QStringLiteral("^#?\\b[a-z0-9-_]+:[a-z0-9-_/.]+\\b"));
    /*qDebug() << str; */
    CurrentNamespacedID currNamespacedID;
    currNamespacedID.startingIndex = startIndex;
    currNamespacedID.blockNumber   = cursor.blockNumber();
    currNamespacedID.string        = namespacedIDRegex.match(str).captured();

    return currNamespacedID;
}

void CodeEditor::followCurrentNamespacedID() {
    QString str = getCurrentNamespacedID().string;

    /*qDebug() << str; */
    if (!str.isEmpty()) {
        QString dirname =
            MainWindow::getCurDir();
        if (dirname.isEmpty()) return;

        QDir dir;
        if (Glhp::removePrefix(str, "#")) {
            dir = QDir(dirname + "/data/" + str.remove(0, 1)
                       .section(":", 0, 0) + "/tags");
        } else {
            dir = QDir(dirname + "/data/" + str.section(":", 0, 0));
        }
        if (!dir.exists()) return;

        QStringList dirList =
            dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
        QString path;
        QString fileExt;
        for (const auto& dirType: dirList) {
            /*qDebug() << "dirType: " << dirType; */
            path = dir.path() + "/" + dirType + "/" + str.section(":", 1, 1);
            if (dirType == "functions" && QFile::exists(path + ".mcfunction")) {
                fileExt = ".mcfunction";
                break;
            } else if (QFile::exists(path + ".json")) {
                fileExt = ".json";
                break;
            }
        }

        /*qDebug() << "final path: " << path; */
        if (!path.isEmpty() && !fileExt.isEmpty()) {
            path              += fileExt;
            this->prevFilepath = path;
            qobject_cast<DatapackTreeView*>(
                qobject_cast<QSplitter*>(this->parent()->parent())->widget(0))
            ->openFromPath(path);
        }
    }
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
