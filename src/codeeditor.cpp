#include "codeeditor.h"

#include "linenumberarea.h"
#include "datapacktreeview.h"
#include "mainwindow.h"
#include "globalhelpers.h"
#include "QFindDialogs/src/finddialog.h"
#include "QFindDialogs/src/findreplacedialog.h"
#include "stripedscrollbar.h"

#include <QPainter>
#include <QFileInfo>
#include <QMimeData>
#include <QFont>
#include <QJsonDocument>
#include <QShortcut>
#include <QToolTip>
#include <QTextDocumentFragment>
#include <QGuiApplication>
#include <QTimer>
#include <QPlainTextDocumentLayout>
#include <QScroller>
#include <QScrollBar>
#include <QCompleter>
#include <QStringListModel>
#include <QElapsedTimer>


TextFileData::TextFileData(QTextDocument *doc, CodeFile *parent) {
    this->parent = parent;
    this->doc    = doc;
    doc->setDocumentLayout(new QPlainTextDocumentLayout(doc));
    this->textCursor = QTextCursor(doc);
}

QStringList getMinecraftInfoKeys(const QString &key) {
    QElapsedTimer timer;

    timer.start();

    QVariantMap infoMap;

    if (key == "block" || key == "item") {
        infoMap = MainWindow::getMCRInfo(key);
    } else {
        infoMap = MainWindow::readMCRInfo(key);
    }
/*
      qDebug() << key << infoMap.size();
      qDebug() << "getMinecraftInfoKeys() exec time:" << timer.nsecsElapsed() /
          1e6;
 */
    return infoMap.keys();
}

QStringList loadMinecraftCommandLiterals(const QJsonObject &obj = QJsonObject(),
                                         ushort depth           = 0) {
    QElapsedTimer timer;

    timer.start();

    QJsonObject root;

    if (obj.isEmpty()) {
        if (depth == 0) {
            QFile file(QStringLiteral(":/minecraft/")
                       + MainWindow::getCurGameVersion().toString()
                       + QStringLiteral(
                           "/mcdata/generated/reports/commands.json"));
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            QJsonParseError errorPtr{};
            QJsonDocument &&doc =
                QJsonDocument::fromJson(file.readAll(), &errorPtr);
            file.close();
            if (doc.isNull()) {
                qWarning() << "Cannot read file" << file.fileName() << ':' <<
                    errorPtr.errorString();
                return QStringList();
            }
            root = doc.object();
        } else {
            return QStringList();
        }
    } else {
        root = obj;
    }

    const auto &&children = root[QStringLiteral("children")].toObject();
    QStringList  ret;
    for (auto it = children.constBegin(); it != children.constEnd(); ++it) {
        const auto   &&node     = it.value().toObject();
        const QString &nodeType = node[QStringLiteral("type")].toString();
        if (nodeType == QStringLiteral("literal")) {
            ret << it.key();
            /*qDebug() << "literal:" << it.key() << "depth:" << depth; */
            ret += loadMinecraftCommandLiterals(node, depth + 1);
        } else if (nodeType == QStringLiteral("argument")) {
            /*qDebug() << "argument:" << it.key() << "depth:" << depth; */
            ret += loadMinecraftCommandLiterals(node, depth + 1);
        }
    }
/*
      qDebug() << "function literals" << ret.size();
      qDebug() << "loadMinecraftCommandLiterals() exec time:" <<
          timer.nsecsElapsed() / 1e6;
 */
    return ret;
}

QStringList loadMinecraftCompletionInfo() {
    QStringList &&ret = loadMinecraftCommandLiterals();

    ret += getMinecraftInfoKeys(QStringLiteral("attribute"));
    ret += getMinecraftInfoKeys(QStringLiteral("block"));
    ret += getMinecraftInfoKeys(QStringLiteral("dimension"));
    ret += getMinecraftInfoKeys(QStringLiteral("enchantment"));
    ret += getMinecraftInfoKeys(QStringLiteral("fluid"));
    ret += getMinecraftInfoKeys(QStringLiteral("feature"));
    ret += getMinecraftInfoKeys(QStringLiteral("entity"));
    ret += getMinecraftInfoKeys(QStringLiteral("item"));
    ret += getMinecraftInfoKeys(QStringLiteral("tag/block"));
    ret += getMinecraftInfoKeys(QStringLiteral("tag/entity_type"));
    ret += getMinecraftInfoKeys(QStringLiteral("tag/fluid"));
    ret += getMinecraftInfoKeys(QStringLiteral("tag/item"));

    ret.sort(Qt::CaseInsensitive);
    return std::move(ret);
}

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {
    lineNumberArea = new LineNumberArea(this);
    setAttribute(Qt::WA_Hover);
    setVerticalScrollBar(new StripedScrollBar(Qt::Vertical, this));

    connect(this, &CodeEditor::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest,
            this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged,
            this, &CodeEditor::onCursorPositionChanged);
    connect(this, &CodeEditor::undoAvailable,
            this, &CodeEditor::onUndoAvailable);
    connect(this, &CodeEditor::redoAvailable,
            this, &CodeEditor::onRedoAvailable);

    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this),
            &QShortcut::activated, this, &CodeEditor::openFindDialog);
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_H), this),
            &QShortcut::activated, this, &CodeEditor::openReplaceDialog);
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Slash), this),
            &QShortcut::activated, this, &CodeEditor::toggleComment);

    /*readPrefSettings(); */

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

    updateLineNumberAreaWidth(0);
    onCursorPositionChanged();
}

void CodeEditor::readPrefSettings() {
    {
        auto *completer = new QCompleter(this);
        minecraftCompletionInfo = loadMinecraftCompletionInfo();
        completer->setModel(new QStringListModel(minecraftCompletionInfo,
                                                 this));
        completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setWrapAround(false);
        setCompleter(completer);

        qDebug() << minecraftCompletionInfo.size() <<
            m_completer->model()->rowCount() <<
            m_completer->completionModel()->rowCount();
    }

    settings.beginGroup("editor");

    if (settings.contains("textFont")) {
        monoFont = qvariant_cast<QFont>(settings.value("textFont"));
    } else {
        monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    }
    monoFont.setStyleHint(QFont::Monospace);
    monoFont.setFixedPitch(true);
    monoFont.setPointSize(settings.value("textSize", 13).toInt());

    setFont(monoFont);

    if (m_completer)
        m_completer->popup()->setFont(monoFont);

    setLineWrapMode(settings.value("wrap", false).toBool()
                        ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
    setTabStopDistance(fontMetrics().horizontalAdvance(
                           QString(' ').repeated(settings.value("tabSize",
                                                                4).toInt())));

    QTextOption &&option = document()->defaultTextOption();
    using Flag = QTextOption::Flag;
    option.setFlags(settings.value("showSpacesAndTabs", false).toBool()
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
        emit showMessageRequest(tr("Zoom: %1%").arg(fontInfo().pointSize() * 100
                                                    / monoFont.pointSize()),
                                1500);
    } else {
        QPlainTextEdit::wheelEvent(e);
    }
}

void CodeEditor::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                      lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::mousePressEvent(QMouseEvent *e) {
    QPlainTextEdit::mousePressEvent(e);

    if (QGuiApplication::keyboardModifiers() & Qt::ControlModifier
        || e->modifiers() & Qt::ControlModifier) {
        followNamespacedId(e);
    }
}

void startOfWordExtended(QTextCursor &tc) {
    static const QString extendedAcceptedCharsset("#.:/");

    while (true) {
        /* Move cursor to the left of the word */
        tc.select(QTextCursor::WordUnderCursor);
        tc.setPosition(tc.anchor());

        /* Get the character to the left of the cursor */
        tc.movePosition(QTextCursor::PreviousCharacter,
                        QTextCursor::KeepAnchor);
        const QChar &curChar = *tc.selectedText().constBegin();

        if (extendedAcceptedCharsset.contains(curChar)) {
            tc.movePosition(QTextCursor::PreviousCharacter);
            tc.movePosition(QTextCursor::PreviousWord);
            tc.movePosition(QTextCursor::NextCharacter);
        } else {
            tc.movePosition(QTextCursor::NextCharacter);
            return;
        }
    }
}

void CodeEditor::handleKeyPressEvent(QKeyEvent *e) {
    if (settings.value(QStringLiteral("editor/insertTabAsSpaces"), true)
        .toBool()) {
        /* Handle Tab and Backtab keys */

        const int tabSize =
            settings.value(QStringLiteral("editor/tabSize"), 4).toInt();
        auto cursor = textCursor();
        if (e->key() == Qt::Key_Tab) {
            cursor.insertText(QString(' ').repeated(tabSize));
            setTextCursor(cursor);
        } else if (e->key() == Qt::Key_Backtab) {
            cursor.beginEditBlock();
            const QString &&line = cursor.block().text();
            for (int i = tabSize; i > 0; --i) {
                if (cursor.atBlockStart())
                    break;
                const QChar &&curChar = line[cursor.positionInBlock() - 1];
                if (!curChar.isSpace())
                    break;
                cursor.deletePreviousChar();
            }
            cursor.endEditBlock();
            setTextCursor(cursor);
        } else {
            goto base;
        }
    } else {
 base:
        if ((e->key() == Qt::Key_Insert) &&
            (e->modifiers() == Qt::NoModifier)) {
            setOverwriteMode(!overwriteMode());
            emit updateStatusBarRequest(this);
        }
        QPlainTextEdit::keyPressEvent(e);
    }
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

    const bool isShortcut =
        (e->modifiers().testFlag(Qt::ControlModifier) &&
         e->key() == Qt::Key_Space);                                                                     /* CTRL+Space */
    /* Do not process the shortcut when we have a completer */
    if (!m_completer || !isShortcut)
        handleKeyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);
    if (!m_completer || (ctrlOrShift && e->text().isEmpty()))
        return;

    const static QString eow("~@$%^&*()+{}|\"<>?,;'[]\\-="); /* end of word */
    const bool           hasModifier =
        (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    const QString &&completionPrefix = textUnderCursor();

    if (!isShortcut &&
        (hasModifier || e->text().isEmpty() || completionPrefix.length() < 3
         || eow.contains(e->text().right(1)))) {
        m_completer->popup()->hide();
        return;
    }

    if (completionPrefix != m_completer->completionPrefix()) {
        if (m_completer->popup()->isHidden()) {
            QStringList completionInfo = minecraftCompletionInfo;
            completionInfo += Glhp::fileIdList(QDir::currentPath(), QString(),
                                               QString(), false).toList();
            completionInfo.removeDuplicates();
            completionInfo.sort(Qt::CaseInsensitive);
            if (auto *model =
                    qobject_cast<QStringListModel*>(m_completer->model())) {
                model->setStringList(completionInfo);
            }
            qDebug() << minecraftCompletionInfo.size() <<
                m_completer->model()->rowCount() <<
                m_completer->completionModel()->rowCount();
        }

        m_completer->setCompletionPrefix(completionPrefix);
        m_completer->popup()->setCurrentIndex(
            m_completer->completionModel()->index(0, 0));
    }

    QRect     cr = cursorRect();
    const int prefixOffset
        = fontMetrics().horizontalAdvance(completionPrefix,
                                          completionPrefix.size() - 1);
    cr.translate(-prefixOffset + cr.width(), 2);
    cr.setWidth(m_completer->popup()->sizeHintForColumn(0)
                + m_completer->popup()->verticalScrollBar()->sizeHint().width());

    m_completer->complete(cr);      /* popup it up! */
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
        /*qDebug() << "QEvent::ToolTip"; */

        auto *helpEvent = static_cast<QHelpEvent*>(event);
        auto  globalPos = mapToGlobal(helpEvent->pos());
        auto  pos       = helpEvent->pos();
        bool  done      = false;

        pos.rx() -= viewportMargins().left();
        pos.ry() -= viewportMargins().top();
        auto &&cursor    = cursorForPosition(pos);
        auto &&block     = cursor.block();
        int    cursorPos = cursor.positionInBlock();

        for (const auto &selection: qAsConst(problemExtraSelections)) {
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
/*
              QToolTip::showText(globalPos,
                                 QString("%1 %2").arg(cursor.selectedText(),
                                                      QString::number(cursor.
                                                                      selectedText()
                                                                      .length())));
 */
        } else {
            QToolTip::hideText();
        }
        /*qDebug() << QToolTip::text() << QToolTip::isVisible(); */

        return true;
    } else {
        return QPlainTextEdit::event(event);
    }
}

void CodeEditor::focusInEvent(QFocusEvent *e) {
    if (m_completer)
        m_completer->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
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
    verticalScrollBar()->update();
    emit updateStatusBarRequest(this);
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
    auto *doc = document();

    doc->setDefaultFont(font());
    settings.beginGroup("editor");
    /* The tab stop distance must be reset each time the current document is changed */
    if (settings.value("insertTabAsSpaces", true).toBool()) {
        setTabStopDistance(fontMetrics().horizontalAdvance(
                               QString(' ').repeated(settings.value("tabSize",
                                                                    4).toInt())));
    }
    QTextOption &&option = doc->defaultTextOption();
    using Flag = QTextOption::Flag;
    option.setFlags(settings.value("showSpacesAndTabs", false).toBool()
                            ? (option.flags() | Flag::ShowTabsAndSpaces)
                            : option.flags() & ~Flag::ShowTabsAndSpaces);
    doc->setDefaultTextOption(option);
    settings.endGroup();
}

int CodeEditor::lineNumberAreaWidth() {
    int digits = 1;
    int max    = qMax(1, blockCount());

    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 6 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

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
        curHighlighter->singleCommentHighlightRules.isEmpty())
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
        curHighlighter->singleCommentHighlightRules.cbegin().key();

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

void CodeEditor::onUndoAvailable(bool value) {
    canUndo = value;
}

void CodeEditor::onRedoAvailable(bool value) {
    canRedo = value;
}

void CodeEditor::insertCompletion(const QString &completion) {
    if (m_completer->widget() != this)
        return;

    QTextCursor tc     = textCursor();
    const int   oldPos = tc.position();
    startOfWordExtended(tc);
    tc.setPosition(oldPos, QTextCursor::KeepAnchor);
    tc.insertText(completion);
    setTextCursor(tc);
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

    m_completer->setWidget(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(m_completer,
                     QOverload<const QString &>::of(&QCompleter::activated),
                     this, &CodeEditor::insertCompletion);
}

QCompleter *CodeEditor::completer() const {
    return m_completer;;
}

bool CodeEditor::getCanUndo() const {
    return canUndo;
}

void CodeEditor::setCurHighlighter(Highlighter *value) {
    curHighlighter = value;
}

void CodeEditor::displayErrors() {
    /*qDebug() << "CodeEditor::displayErrors"; */
    if (!isReadOnly()) {
        auto &&selections = extraSelections().mid(0,
                                                  problemSelectionStartIndex +
                                                  1);
        selections += problemExtraSelections;

        setExtraSelections(selections);
        if (auto *scrollbar =
                qobject_cast<StripedScrollBar*>(verticalScrollBar())) {
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
        if (!document() || !curHighlighter)
            return;

        problemExtraSelections.clear();

        for (auto it = document()->firstBlock();
             it != document()->end(); it = it.next()) {
            /*qDebug() << it.blockNumber(); */
            if (TextBlockData *data =
                    dynamic_cast<TextBlockData *>(it.userData())) {
/*
                  qDebug() << it.blockNumber() << data <<
                      data->problem().has_value();
 */
                if (auto &&problem = data->problem(); problem) {
                    QTextEdit::ExtraSelection selection;
                    selection.cursor = textCursor();
                    selection.cursor.setPosition(problem->start);
                    if (problem->length > 0) {
                        if (selection.cursor.atBlockEnd()) {
                            selection.cursor.select(QTextCursor::LineUnderCursor);
                        } else {
                            selection.cursor.setPosition(
                                problem->start + problem->length,
                                QTextCursor::KeepAnchor);
                        }
                    } else {
                        selection.cursor.select(QTextCursor::WordUnderCursor);
                    }
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

    painter.fillRect(event->rect(), palette().midlight());
    /*
       painter.setPen(QColor(240, 240, 240));
       painter.drawLine(event->rect().topRight(), event->rect().bottomRight());
     */
    QTextBlock &&block       = firstVisibleBlock();
    int          blockNumber = block.blockNumber();
    int          top         =
        qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString &&number = QString::number(blockNumber + 1);
            if (blockNumber == this->textCursor().blockNumber()) {
                painter.setPen(palette().shadow().color());
            } else {
                painter.setPen(palette().mid().color());
            }
            painter.drawText(0, top, lineNumberArea->width() - 3,
                             fontMetrics().height(), Qt::AlignRight, number);
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

    QVector<BracketInfo*> && infos = data->brackets();

    int pos = textCursor().block().position();

    for (int i = 0; i < infos.size(); ++i) {
        const BracketInfo *info = infos.at(i);

        int curPos = textCursor().position() -
                     textCursor().block().position();
        /* info->position == curPos:
         *     the text cursor is on the left of the character
         * info->position == curPos - 1
         *     the text cursor is on the right of the character */
        bool isOnTheLeftOfChar = info->pos == curPos;

        if (isOnTheLeftOfChar || (info->pos == curPos - 1)) {
            for (const auto &pair: qAsConst(curHighlighter->bracketPairs)) {
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
    setExtraSelections(std::move(selections));
}

void CodeEditor::followNamespacedId(const QMouseEvent *event) {
    TextBlockData *data =
        dynamic_cast<TextBlockData *>(textCursor().block().userData());

    if (!data || !curHighlighter)
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

QString textUnderCursorExtended(QTextCursor tc) {
    const int oldPos = tc.position();

    startOfWordExtended(tc);
    tc.setPosition(oldPos, QTextCursor::KeepAnchor);

    qDebug() << "textUnderCursorExtended" << tc.selectedText();
    return tc.selectedText();
}

QString CodeEditor::textUnderCursor() const {
    QTextCursor tc = textCursor();

    return textUnderCursorExtended(tc);
}
