#include "codeeditor.h"

#include "codegutter.h"
#include "highlighter.h"
#include "globalhelpers.h"
#include "QFindDialogs/src/finddialog.h"
#include "QFindDialogs/src/findreplacedialog.h"
#include "stripedscrollbar.h"
#include "parsers/command/mcfunctionparser.h"
#include "parsers/command/visitors/completionprovider.h"
#include "stringvectormodel.h"

#include <QPainter>
#include <QMimeData>
#include <QFont>
#include <QJsonDocument>
#include <QShortcut>
#include <QToolTip>
#include <QTextDocumentFragment>
#include <QApplication>
#include <QTimer>
#include <QPlainTextDocumentLayout>
#include <QScroller>
#include <QScrollBar>
#include <QCompleter>
#include <QElapsedTimer>
#include <QAbstractItemView>
#include <QMenu>


// Adapted from: https://stackoverflow.com/a/56678483/12682038
qreal perceivedLightness(const QColor &color) {
    const static auto sRGBtoLin =
        [](qreal colorChannel) {
            if (colorChannel <= 0.04045) {
                return colorChannel / 12.92;
            } else {
                return pow(((colorChannel + 0.055) / 1.055), 2.4);
            }
        };

    const qreal vR = color.redF();
    const qreal vG = color.greenF();
    const qreal vB = color.blueF();

    const qreal Y = (0.2126 * sRGBtoLin(vR) + 0.7152 * sRGBtoLin(vG)
                     + 0.0722 * sRGBtoLin(vB));

    if (Y <= (216. / 24389)) {
        return Y * (24389. / 27);
    } else {
        return pow(Y, (1. / 3)) * 116 - 16;
    }
}

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {
    m_gutter = new CodeGutter(this);
    setAttribute(Qt::WA_Hover);
    setVerticalScrollBar(new StripedScrollBar(Qt::Vertical, this));
    readPrefSettings();

    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &CodeEditor::updateGutterWidth);
    connect(this, &QPlainTextEdit::updateRequest,
            this, &CodeEditor::updateGutter);
    connect(this, &QPlainTextEdit::cursorPositionChanged,
            this, &CodeEditor::onCursorPositionChanged);
    connect(this, &QPlainTextEdit::undoAvailable,
            this, &CodeEditor::onUndoAvailable);
    connect(this, &QPlainTextEdit::redoAvailable,
            this, &CodeEditor::onRedoAvailable);
    connect(document(), &QTextDocument::contentsChanged,
            this, &CodeEditor::onTextChanged);

    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this),
            &QShortcut::activated, this, &CodeEditor::openFindDialog);
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_H), this),
            &QShortcut::activated, this, &CodeEditor::openReplaceDialog);
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Slash), this),
            &QShortcut::activated, this, &CodeEditor::toggleComment);
    connect(new QShortcut(QKeySequence(
                              Qt::SHIFT + Qt::ALT + Qt::Key_Down), this),
            &QShortcut::activated, this, &CodeEditor::copyLineDown);
    connect(new QShortcut(QKeySequence(
                              Qt::SHIFT + Qt::ALT + Qt::Key_Up), this),
            &QShortcut::activated, this, &CodeEditor::copyLineUp);
    connect(new QShortcut(QKeySequence(
                              Qt::CTRL + Qt::SHIFT + Qt::Key_Down), this),
            &QShortcut::activated, this, &CodeEditor::moveLineDown);
    connect(new QShortcut(QKeySequence(
                              Qt::CTRL + Qt::SHIFT + Qt::Key_Up), this),
            &QShortcut::activated, this, &CodeEditor::moveLineUp);
    connect(new QShortcut(QKeySequence(
                              Qt::CTRL + Qt::Key_L), this),
            &QShortcut::activated, this, &CodeEditor::selectCurrentLine);

    bracketSeclectFmt.setFontWeight(QFont::Bold);
    bracketSeclectFmt.setForeground(Qt::red);
    bracketSeclectFmt.setBackground(QColor(102, 227, 102, 170));

    QScroller::grabGesture(viewport(), QScroller::MiddleMouseButtonGesture);

    errorHighlightRule.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    errorHighlightRule.setUnderlineColor(Qt::red);

/*
      errorHighlightRule.setBackground(QColor(255, 127, 127, 63));
      errorHighlightRule.setProperty(QTextFormat::FullWidthSelection,
                                     true);
 */

    updateGutterWidth(0);
    onCursorPositionChanged();
}

void CodeEditor::setFileType(CodeFile::FileType type) {
    m_fileType = type;

    initCompleter();
}

void CodeEditor::initCompleter() {
    auto *completer = new QCompleter(this);

    if (m_fileType == CodeFile::Function) {
//        if (minecraftCompletionInfo.isEmpty()) {
//            minecraftCompletionInfo = loadMinecraftCompletionInfo();
//            qDebug() << "Minecraft completions loaded (" <<
//                minecraftCompletionInfo.size() << "items)";
//        }
    }
    completer->setModel(new StringVectorModel(this));
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);
    setCompleter(completer);
}

void CodeEditor::readPrefSettings() {
    QSettings settings;

    settings.beginGroup("editor");

    QFont monoFont;
    if (settings.contains("textFont")) {
        monoFont = qvariant_cast<QFont>(settings.value("textFont"));
    } else {
        monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    }
    monoFont.setStyleHint(QFont::Monospace);
    monoFont.setFixedPitch(true);
    monoFont.setPointSize(settings.value("textSize", 13).toInt());

    setFont(monoFont);
    document()->setDefaultFont(monoFont);
    m_fontSize = monoFont.pointSize();

    if (m_completer)
        m_completer->popup()->setFont(monoFont);

    if (m_gutter)
        m_gutter->setFont(font());

    setLineWrapMode(settings.value("wrap", false).toBool()
                        ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
    m_tabSize = settings.value("tabSize", 4).toInt();
    setTabStopDistance(fontMetrics().horizontalAdvance(QString(' ').repeated(
                                                           m_tabSize)));

    QTextOption &&option = document()->defaultTextOption();
    using Flag          = QTextOption::Flag;
    m_insertTabAsSpaces = settings.value("showSpacesAndTabs", false).toBool();
    option.setFlags(m_insertTabAsSpaces
                        ? (option.flags() | Flag::ShowTabsAndSpaces)
                        : option.flags() & ~Flag::ShowTabsAndSpaces);
    document()->setDefaultTextOption(option);
    settings.endGroup();
}

void CodeEditor::wheelEvent(QWheelEvent *e) {
    if (e->modifiers().testFlag(Qt::ControlModifier)) {
        const int delta = e->angleDelta().y() / 120;
        if (delta != 0)
            zoomIn(delta); /* Zoom out when delta < 0 */
        if (m_completer)
            m_completer->popup()->setFont(font());
        if (m_gutter)
            m_gutter->setFont(font());
        emit showMessageRequest(tr("Zoom: %1%").arg(fontInfo().pointSize() * 100
                                                    / m_fontSize),
                                1500);
    } else {
        QPlainTextEdit::wheelEvent(e);
    }
}

void CodeEditor::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    m_gutter->adjustSize();

    QRect cr = contentsRect();
    m_gutter->setGeometry(QRect(cr.left(), cr.top(),
                                m_gutter->size().width(), cr.height()));
    m_gutter->setFixedHeight(cr.height());
}

void CodeEditor::mousePressEvent(QMouseEvent *e) {
    QPlainTextEdit::mousePressEvent(e);

    if (QApplication::keyboardModifiers() & Qt::ControlModifier
        || e->modifiers() & Qt::ControlModifier) {
        followNamespacedId(e);
    }
}

void CodeEditor::mouseDoubleClickEvent(QMouseEvent *e) {
    if (QApplication::keyboardModifiers() & Qt::ShiftModifier
        || e->modifiers() & Qt::ShiftModifier) {
        QTextCursor tc = textCursor();
        //tc.select(QTextCursor::WordUnderCursor);
        tc.movePosition(QTextCursor::EndOfWord);
        const int oldPos = tc.position();
        tc = textCursor(); // Reuse initial position
        startOfWordExtended(tc);
        tc.setPosition(oldPos, QTextCursor::KeepAnchor);
        setTextCursor(tc);
        e->accept();
    } else {
        QPlainTextEdit::mouseDoubleClickEvent(e);
    }
}

void debugTextCursor(const QTextCursor &tc) {
    //return;

    const QString reprTemplate("'%1|%2' (pos: %3)");
    const QString reprTemplate2("'%1%2%3%4%5' (anc: %6, pos: %7)");

    const QString &&line   = tc.block().text();
    const int       pos    = tc.positionInBlock();
    const int       anchor = tc.anchor() - tc.block().position();

    if (pos == anchor) {
        qDebug() <<
            reprTemplate.arg(line.leftRef(pos)).arg(line.midRef(pos)).arg(pos);
    } else {
        const int   left  = qMin(pos, anchor);
        const int   right = qMax(pos, anchor);
        const QChar lchar = (pos > anchor) ? '[' : '|';
        const QChar rchar = (pos > anchor) ? '|' : ']';

        qDebug() << reprTemplate2.arg(line.leftRef(left)).arg(lchar)
            .arg(line.midRef(left, right - left)).arg(rchar)
            .arg(line.midRef(right)).arg(anchor).arg(pos);
    }
}

void CodeEditor::startOfWordExtended(QTextCursor &tc) const {
    static const QString extendedAcceptedCharsset("#.:/-");

    //debugTextCursor(tc);

    while (true) {
        /* Move cursor to the left of the word */
        tc.movePosition(QTextCursor::StartOfWord);
        //debugTextCursor(tc);

        /* Get the character to the left of the cursor */
        tc.movePosition(QTextCursor::PreviousCharacter,
                        QTextCursor::KeepAnchor);
        //debugTextCursor(tc);
        // This fixes garbage characters in debug mode
        const QString &&selectedText = tc.selectedText();
        if (selectedText.isEmpty()) {
            return;
        }
        const QChar &curChar = selectedText.front();

        if (extendedAcceptedCharsset.contains(curChar)) {
            tc.movePosition(QTextCursor::PreviousCharacter);
            //debugTextCursor(tc);
            if (!tc.atBlockStart()) {
                tc.movePosition(QTextCursor::PreviousWord);
                //debugTextCursor(tc);
                tc.movePosition(QTextCursor::NextCharacter);
                //debugTextCursor(tc);
            } else {
                // Prevent infinite loop when '#' is at start of a line
                tc.movePosition(QTextCursor::NextCharacter);
                //debugTextCursor(tc);
                return;
            }
        } else {
            tc.movePosition(QTextCursor::NextCharacter);
            //debugTextCursor(tc);
            return;
        }
    }
}

void CodeEditor::indentOnNewLine(QKeyEvent *e) {
    auto tc = textCursor();

    tc.removeSelectedText();
    setTextCursor(tc);
    const int linePos = tc.positionInBlock();

    tc.select(QTextCursor::LineUnderCursor);
    const QString &&lineText  = tc.selectedText();
    int             curIndent = 0;
    for (const auto &chr: lineText) {
        if (chr == ' ') ++curIndent;
        else break;
    }

    const int destIndent = qMin(curIndent, linePos);

    tc = textCursor();
    bool shouldJoinEdit = true;
    if ((linePos - 1) > -1) {
        const QChar &&prevChar = lineText.at(linePos - 1);
        shouldJoinEdit = prevChar == ' ';
    }

    if (shouldJoinEdit) {
        tc.joinPreviousEditBlock();
        if ((linePos - 1) <= curIndent) {
            /* Avoid inserting redundant spaces */
            tc.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
            tc.removeSelectedText();
        }
    } else {
        tc.beginEditBlock();
    }
    tc.insertText('\n' + QString(' ').repeated(destIndent));
    tc.endEditBlock();
    setTextCursor(tc);
    e->accept();
}

void CodeEditor::handleKeyPressEvent(QKeyEvent *e) {
    if (m_insertTabAsSpaces) {
        /* Handle Tab and Backtab keys */

        auto cursor = textCursor();
        if (e->key() == Qt::Key_Tab) {
            cursor.insertText(QStringLiteral(" ").repeated(m_tabSize));
            setTextCursor(cursor);
            e->accept();
            return;
        } else if (e->key() == Qt::Key_Backtab) {
            cursor.beginEditBlock();
            const QString &&line = cursor.block().text();
            for (int i = m_tabSize; i > 0; --i) {
                if (cursor.atBlockStart())
                    break;
                const QChar &&curChar = line[cursor.positionInBlock() - 1];
                if (!curChar.isSpace())
                    break;
                cursor.deletePreviousChar();
            }
            cursor.endEditBlock();
            setTextCursor(cursor);
            e->accept();
            return;
        }
    }

    if (e->key() == Qt::Key_Return) {
        indentOnNewLine(e);
    } else {
        if ((e->key() == Qt::Key_Insert) &&
            (e->modifiers() == Qt::NoModifier)) {
            setOverwriteMode(!overwriteMode());
            emit updateStatusBarRequest(this);
        }

        QPlainTextEdit::keyPressEvent(e);
    }
}

void CodeEditor::startCompletion(const QString &completionPrefix) {
    if (m_completer->popup()->isHidden()) {
        qDebug() << "Combining final completions";

        QVector<QString> completionInfo;
        if (const auto *parser =
                dynamic_cast<Command::McfunctionParser *>(m_parser.get())) {
            const int curLine =
                parser->syntaxTree()->sourceMapper().logicalLinesIndexOf(
                    textCursor().blockNumber());
            if (curLine != -1) {
                const int posInLine = textCursor().positionInBlock();

                if (auto *line = parser->syntaxTree()->at(curLine).get();
                    line->kind() == Command::ParseNode::Kind::Root) {
                    Command::CompletionProvider suggester{ posInLine };
                    suggester.startVisiting(line);
                    completionInfo = suggester.suggestions();
                    std::sort(completionInfo.begin(), completionInfo.end());
                    completionInfo.erase(std::unique(completionInfo.begin(),
                                                     completionInfo.end()),
                                         completionInfo.end());
                }
            }
        }

        if (auto *model =
                qobject_cast<StringVectorModel *>(m_completer->model())) {
            m_completer->setCompletionPrefix(QString());
            model->setVector(completionInfo);
        }
        //            qDebug() << minecraftCompletionInfo.size() <<
        //                m_completer->model()->rowCount() <<
        //                m_completer->completionModel()->rowCount();
    }

    m_completer->setCompletionPrefix(completionPrefix);
    m_completer->popup()->setCurrentIndex(
        m_completer->completionModel()->index(0, 0));

//    qDebug() << m_completer->completionCount() << m_completer->model() <<
//        completionPrefix;

    QRect     cr = cursorRect();
    const int prefixOffset
        = fontMetrics().horizontalAdvance(completionPrefix,
                                          completionPrefix.size());
    cr.translate(-prefixOffset + cr.width() + viewportMargins().left(), 2);
    cr.setWidth(m_completer->popup()->sizeHintForColumn(0)
                + m_completer->popup()->verticalScrollBar()->sizeHint().width());

    m_needCompleting = false;
    m_completer->complete(cr);       // popup it up!
}

void CodeEditor::keyPressEvent(QKeyEvent *e) {
    if (m_completer && m_completer->popup()->isVisible()) {
        /* The following keys are forwarded by the completer to the widget */
        switch (e->key()) {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Escape:
            case Qt::Key_Tab:
            case Qt::Key_Backtab:
                e->ignore();
                return; /* let the completer do default behavior */

            default:
                break;
        }
    }

    const bool isCompletionShortcut =
        (e->modifiers().testFlag(Qt::ControlModifier) &&
         e->key() == Qt::Key_Space);  /* CTRL+Space */

    /* Do not process the shortcut when we have a completer */
    if (!m_completer || !isCompletionShortcut) {
        handleKeyPressEvent(e);
    }

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);

    if (!m_completer || (ctrlOrShift && e->text().isEmpty())) {
        return;
    }

    const static QString eow(QStringLiteral("~@$%^&*()+{}|\"<>?,;'[]\\-=")); /* end of word */
    const bool           hasModifier =
        (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    const QString &&completionPrefix = textUnderCursor();

    if (!isCompletionShortcut &&
        (hasModifier || e->text().isEmpty() || completionPrefix.length() < 2
         || eow.contains(e->text().right(1)))) {
        m_completer->popup()->hide();
        return;
    } else if (isCompletionShortcut) {
        startCompletion(completionPrefix);
    } else {
        m_needCompleting = true;
    }
}

void CodeEditor::dropEvent(QDropEvent *e) {
    QString nspacedID;

    if (e->mimeData()->hasFormat(QStringLiteral("text/uri-list"))) {
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
        cursor.insertText(nspacedID);
        setTextCursor(cursor);

        auto *mimeData = new QMimeData();
        mimeData->setText(QString());
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

    QAction *sep1Action = new QAction(tr("More"), menu);

    sep1Action->setSeparator(true);
    menu->addAction(sep1Action);

    QAction *formatAction = new QAction(tr("Format"), menu);

    formatAction->setDisabled(!((CodeFile::JsonText <= m_fileType) &&
                                (m_fileType <= CodeFile::JsonText_end)));
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

QPoint translatedMargins(const QPoint &p, const QMargins &margins) {
    return QPoint{ p.x() - margins.left(), p.y() - margins.right() };
}

bool CodeEditor::event(QEvent *event) {
    if (event->type() == QEvent::ToolTip) {
        const auto    *helpEvent = static_cast<QHelpEvent *>(event);
        const QPoint  &globalPos = helpEvent->globalPos();
        const QPoint &&pos       = translatedMargins(helpEvent->pos(),
                                                     viewportMargins());
        bool done = false;

        const auto &&cursor = cursorForPosition(pos);
        if (pos.x() < 0
            || abs(cursorRect(cursor).center().y() - pos.y()) > m_fontSize) {
            QToolTip::hideText();
            event->ignore();
            return false;
        }

        const auto &&block        = cursor.block();
        const int    cursorPos    = cursor.positionInBlock();
        const int    cursorAbsPos = cursor.position();

        for (const auto &selection: qAsConst(problemExtraSelections)) {
            auto selCursor = selection.cursor;
            if ((cursorAbsPos >= selCursor.selectionStart())
                && (cursorAbsPos <= selCursor.selectionEnd())) {
                QRect &&selRect = cursorRect(selCursor);
                selCursor.setPosition(selCursor.selectionEnd());
                selRect |= cursorRect(selCursor);
                QToolTip::showText(globalPos, selection.format.toolTip(),
                                   this, selRect);
                done = true;
                break;
            }
        }
        if (done) {
            event->accept();
            return true;
        }

        const auto &formats = block.layout()->formats();
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
        if (done) {
            event->accept();
            return true;
        }

        QToolTip::hideText();
        event->ignore();
        return false;
    } else {
        return QPlainTextEdit::event(event);
    }
}

void CodeEditor::focusInEvent(QFocusEvent *e) {
    if (m_completer)
        m_completer->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

void CodeEditor::changeEvent(QEvent *e) {
    if (e->type() == QEvent::PaletteChange) {
        if (m_highlighter) {
            if (perceivedLightness(palette().base().color()) < 50) {
                m_highlighter->setPalette(defaultDarkCodePalette);
            } else {
                m_highlighter->setPalette(defaultCodePalette);
            }
            onCursorPositionChanged();
            if (m_highlighter->hasAdvancedHighlighting()) {
                m_highlighter->ensureDelayedRehighlightAll();
                m_highlighter->rehighlightDelayed();
            } else {
                m_highlighter->rehighlight();
            }
        }
    }
    QPlainTextEdit::changeEvent(e);
}

void CodeEditor::onCursorPositionChanged() {
//    qDebug() << "CodeEditor::onCursorPositionChanged";
    setExtraSelections({});
    highlightCurrentLine();
    matchParentheses();
    problemSelectionStartIndex = extraSelections().size() - 1;
    if (!problemExtraSelections.isEmpty()) {
        setExtraSelections(extraSelections() << problemExtraSelections);
    }
    verticalScrollBar()->update();
    emit updateStatusBarRequest(this);
}


void CodeEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> selections = extraSelections();

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        const int factor    = 105;
        QColor    lineColor = palette().base().color();
        if (perceivedLightness(lineColor) < 50) {
            lineColor = lineColor.lighter(factor);
        } else {
            lineColor = lineColor.darker(factor);
        }

        selection.format = QTextCharFormat();
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();

        selections.append(selection);
    }
    setExtraSelections(selections);
}

void CodeEditor::updateGutterWidth(int /* newBlockCount */) {
    m_gutter->updateChildrenGeometries();
    m_gutter->adjustSize();
    setViewportMargins(m_gutter->size().width(), 0, 0, 0);
}

void CodeEditor::updateGutter(const QRect &rect, int dy) {
    if (dy)
        m_gutter->scroll(0, dy, rect);
    else
        m_gutter->update();

    if (rect.contains(viewport()->rect()))
        updateGutterWidth(0);
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
    if (!m_highlighter ||
        m_highlighter->m_singleCommentCharset.isEmpty())
        return;

    auto       txtCursor    = textCursor();
    const auto posBlock     = txtCursor.block();
    const int  posInBlock   = txtCursor.positionInBlock();
    auto       anchorCursor = txtCursor;

    anchorCursor.setPosition(txtCursor.anchor());
    const auto anchorBlock      = anchorCursor.block();
    const int  anchorPosInBlock = anchorCursor.positionInBlock();

    txtCursor.beginEditBlock();
    const bool fromTopDown = txtCursor.position() > txtCursor.anchor();
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
        m_highlighter->m_singleCommentCharset[0];

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

void CodeEditor::copyLineUp() {
    QTextCursor &&cursor = textCursor();

    selectEnclosingLines(cursor);
    if (cursor.hasSelection()) {
        const int       anchor = cursor.anchor();
        const QString &&text   = cursor.selectedText();
        cursor.setPosition(anchor);

        cursor.beginEditBlock();
        cursor.insertText(text);
        const int pos = cursor.position();
        cursor.insertBlock();
        cursor.setPosition(anchor);
        cursor.setPosition(pos, QTextCursor::KeepAnchor);
        cursor.endEditBlock();

        setTextCursor(cursor);
    }
}

void CodeEditor::copyLineDown() {
    QTextCursor &&cursor = textCursor();

    selectEnclosingLines(cursor);
    if (cursor.hasSelection()) {
        const QString &&text = cursor.selectedText();
        cursor.movePosition(QTextCursor::EndOfBlock);

        cursor.beginEditBlock();
        cursor.insertBlock();
        const int anchor = cursor.position();
        cursor.insertText(text);
        const int pos = cursor.position();
        cursor.setPosition(anchor);
        cursor.setPosition(pos, QTextCursor::KeepAnchor);
        cursor.endEditBlock();

        setTextCursor(cursor);
    }
}

void CodeEditor::moveLineUp() {
    QTextCursor &&cursor = textCursor();

    selectEnclosingLines(cursor);
    if (cursor.hasSelection() && !cursor.atStart()) {
        const QString &&text = cursor.selectedText();
        cursor.beginEditBlock();
        cursor.removeSelectedText();
        cursor.deletePreviousChar();
        cursor.movePosition(QTextCursor::StartOfBlock);

        const int anchor = cursor.position();
        cursor.insertText(text);
        const int pos = cursor.position();
        cursor.insertBlock();
        cursor.setPosition(anchor);
        cursor.setPosition(pos, QTextCursor::KeepAnchor);

        cursor.endEditBlock();
        setTextCursor(cursor);
    }
}

void CodeEditor::moveLineDown() {
    QTextCursor &&cursor = textCursor();

    selectEnclosingLines(cursor);
    if (cursor.hasSelection()) {
        const QString &&text = cursor.selectedText();
        cursor.beginEditBlock();
        cursor.removeSelectedText();
        cursor.deleteChar();
        cursor.movePosition(QTextCursor::EndOfBlock);
        cursor.insertBlock();

        const int anchor = cursor.position();
        cursor.insertText(text);
        const int pos = cursor.position();
        cursor.setPosition(anchor);
        cursor.setPosition(pos, QTextCursor::KeepAnchor);

        cursor.endEditBlock();
        setTextCursor(cursor);
    }
}

void CodeEditor::selectCurrentLine() {
    QTextCursor &&cursor = textCursor();

    cursor.select(QTextCursor::LineUnderCursor);
    setTextCursor(cursor);
}

void CodeEditor::onUndoAvailable(bool value) {
    canUndo = value;
}

void CodeEditor::onRedoAvailable(bool value) {
    canRedo = value;
}

void CodeEditor::insertCompletion(const QString &completion) {
    if (m_completer->widget() != this)
        return;

    m_needCompleting = false;

    QTextCursor tc     = textCursor();
    const int   oldPos = tc.position();
    startOfWordExtended(tc);
    tc.setPosition(oldPos, QTextCursor::KeepAnchor);
    tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    tc.insertText(completion);
    setTextCursor(tc);
}

void CodeEditor::onTextChanged() {
//    qDebug() << "CodeEditor::onTextChanged";
    if (m_parser) {
        bool ok = m_parser->parse(toPlainText());
        m_problems.clear();
        if (!ok) {
            m_problems.reserve(m_parser->errors().size());
            for (const auto &error: m_parser->errors()) {
                ProblemInfo problem{ ProblemInfo::Type::Error,
                                     error.pos, error.length,
                                     error.toLocalizedMessage() };
                m_problems << std::move(problem);
            }
        }
        if (m_highlighter) {
            m_highlighter->rehighlightDelayed();
        }
        if (m_needCompleting && m_completer) {
            startCompletion(textUnderCursor());
        }
        updateErrorSelections();
    }
}

Parser * CodeEditor::parser() const {
    return m_parser.get();
}

void CodeEditor::goToLine(const int lineNo) {
    auto &&cursor = textCursor();

    cursor.setPosition(document()->findBlockByLineNumber(lineNo).position());
    setTextCursor(cursor);
    centerCursor();
}

void CodeEditor::setParser(std::unique_ptr<Parser> newParser) {
    m_parser = std::move(newParser);
}

Highlighter * CodeEditor::highlighter() const {
    return m_highlighter;
}

bool CodeEditor::getCanRedo() const {
    return canRedo;
}

int CodeEditor::problemCount() const {
    return problemExtraSelections.size();
}

void CodeEditor::setCompleter(QCompleter *c) {
    if (m_completer)
        m_completer->disconnect(this);

    m_completer = c;

    if (!m_completer)
        return;

    m_completer->popup()->setFont(font());
    m_completer->setWidget(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(m_completer,
                     qOverload<const QString &>(&QCompleter::activated),
                     this, &CodeEditor::insertCompletion);
}

QCompleter * CodeEditor::completer() const {
    return m_completer;
}

bool CodeEditor::getCanUndo() const {
    return canUndo;
}

void CodeEditor::setHighlighter(Highlighter *value) {
    m_highlighter = value;
    if (perceivedLightness(palette().base().color()) < 50) {
        m_highlighter->setPalette(defaultDarkCodePalette);
    } else {
        m_highlighter->setPalette(defaultCodePalette);
    }
}

void CodeEditor::displayErrors() {
    /*qDebug() << "CodeEditor::displayErrors"; */
    if (!isReadOnly()) {
        auto selections = extraSelections().
                          mid(0, problemSelectionStartIndex + 1);
        selections += problemExtraSelections;

        setExtraSelections(selections);
        if (auto *scrollbar =
                qobject_cast<StripedScrollBar *>(verticalScrollBar())) {
            scrollbar->redrawStripes();
            scrollbar->update();
        }
        emit updateStatusBarRequest(this);
    }
    QToolTip::hideText();
}

void CodeEditor::updateErrorSelections() {
    /*qDebug() << "CodeEditor::updateErrorSelections"; */
    if (!isReadOnly()) {
        if (!document() || !m_parser)
            return;

        problemExtraSelections.clear();

        QTextCursor tc = textCursor();
        tc.movePosition(QTextCursor::End);
        const int endPos = tc.position();

        for (const auto &problem: m_problems) {
            QTextEdit::ExtraSelection selection;
            QTextCursor               selCursor = textCursor();
            selCursor.setPosition(qBound(0, problem.pos, endPos));

            if (problem.length > 0) {
                if (selCursor.atBlockEnd()) {
                    selCursor.select(QTextCursor::LineUnderCursor);
                } else {
                    selCursor.setPosition(
                        selCursor.position() + problem.length,
                        QTextCursor::KeepAnchor);
                }
            } else {
                selCursor.select(QTextCursor::WordUnderCursor);
            }
            selection.cursor = selCursor;
            selection.format = errorHighlightRule;
            selection.format.setToolTip(problem.message);
            problemExtraSelections << selection;
        }
    }
    displayErrors();
}

void CodeEditor::matchParentheses() {
    /*bool match = false; */

    TextBlockData *data =
        dynamic_cast<TextBlockData *>(textCursor().block().userData());

    if (!m_highlighter || !data)
        return;

    const QVector<BracketInfo *> && infos = data->brackets();

    int pos = textCursor().block().position();

    for (int i = 0; i < infos.size(); ++i) {
        const BracketInfo *info = infos.at(i);

        int curPos = textCursor().positionInBlock();
        /* info->position == curPos:
         *     the text cursor is on the left of the character
         * info->position == curPos - 1
         *     the text cursor is on the right of the character */
        bool isOnTheLeftOfChar = info->pos == curPos;

        if (isOnTheLeftOfChar || (info->pos == curPos - 1)) {
            for (const auto &pair: qAsConst(m_highlighter->bracketPairs)) {
                if (info->character == pair.left) {
                    if (matchLeftBracket(textCursor().block(), i + 1,
                                         pair.left, pair.right,
                                         0, isOnTheLeftOfChar)) {
                        createBracketSelection(pos + info->pos,
                                               isOnTheLeftOfChar);
                    }
                } else if (info->character == pair.right) {
                    if (matchRightBracket(textCursor().block(), i - 1,
                                          pair.right, pair.left,
                                          0, !isOnTheLeftOfChar)) {
                        createBracketSelection(pos + info->pos,
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

    QVector<BracketInfo *> && infos = data->brackets();

    int docPos = currentBlock.position();

    for (; i < infos.size(); ++i) {
        BracketInfo *info = infos.at(i);

        if (info->character == chr) {
            ++numLeftParentheses;
            continue;
        }

        if (info->character == corresponder) {
            if (numLeftParentheses == 0) {
                createBracketSelection(docPos + info->pos, isPrimary);

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

    QVector<BracketInfo *> && infos = data->brackets();

    if (i == -2)
        i = infos.count() - 1;

    int docPos = currentBlock.position();

    for (; i > -1 && !infos.isEmpty(); --i) {
        BracketInfo *info = infos.at(i);
        if (info->character == chr) {
            ++numRightParentheses;
            continue;
        }

        if (info->character == corresponder) {
            if (numRightParentheses == 0) {
                createBracketSelection(docPos + info->pos, isPrimary);
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
    QList<QTextEdit::ExtraSelection> && selections = extraSelections();

    QTextEdit::ExtraSelection selection;

    selection.format = bracketSeclectFmt;
    if (!isPrimary)
        selection.format.setBackground(
            selection.format.background().color().lighter());

    QTextCursor &&cursor = textCursor();
    cursor.setPosition(pos);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    selection.cursor = std::move(cursor);

    selections << selection;
    setExtraSelections(selections);
}

void CodeEditor::followNamespacedId(const QMouseEvent *event) {
    TextBlockData *data =
        dynamic_cast<TextBlockData *>(textCursor().block().userData());

    if (!data || !m_highlighter)
        return;

    QVector<NamespacedIdInfo *> infos = data->namespacedIds();

    /*qDebug() << "followNamespacedId" << infos.count(); */

    const auto &&cursor         = cursorForPosition(event->pos());
    const auto &&cursorBlockPos = cursor.positionInBlock();

    for (const auto &info: infos) {
        if (cursorBlockPos >= info->start &&
            cursorBlockPos <= (info->start + info->length)) {
            emit openFileRequest(info->link);
            break;
        }
    }
}

QString CodeEditor::textUnderCursorExtended(QTextCursor tc) const {
    const int oldPos = tc.position();

    startOfWordExtended(tc);
    tc.setPosition(oldPos, QTextCursor::KeepAnchor);

    //qDebug() << "textUnderCursorExtended" << tc.selectedText();
    return tc.selectedText();
}

void CodeEditor::selectEnclosingLines(QTextCursor &cursor) const {
    if (cursor.hasSelection()) {
        int selStart = cursor.selectionStart();
        int selEnd   = cursor.selectionEnd();
        cursor.setPosition(selStart);
        cursor.movePosition(QTextCursor::StartOfBlock);
        selStart = cursor.position();
        cursor.setPosition(selEnd);
        cursor.movePosition(QTextCursor::EndOfBlock);
        selEnd = cursor.position();
        cursor.setPosition(selStart);
        cursor.setPosition(selEnd, QTextCursor::KeepAnchor);
    } else {
        // cursor.select(QTextCursor::BlockUnderCursor) will move the cursor to
        // the first position of the next block.
        cursor.select(QTextCursor::LineUnderCursor);
    }
}

QString CodeEditor::textUnderCursor() const {
    return textUnderCursorExtended(textCursor());
}
