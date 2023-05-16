#include "rawjsontextedit.h"

#include "rawjsontextobjectinterface.h"
#include "globalhelpers.h"

#include <QTextDocument>
#include <QPainter>
#include <QScrollBar>
#include <QTextDocumentFragment>
#include <QMimeData>

QDataStream &operator<<(QDataStream &out,
                        const QTextLayout::FormatRange &obj) {
    out << obj.start << obj.length << obj.format;
    return out;
}

QDataStream &operator>>(QDataStream &in, QTextLayout::FormatRange &obj) {
    in >> obj.start >> obj.length >> obj.format;
    return in;
}

RawJsonTextEdit::RawJsonTextEdit(QWidget *parent) : QTextEdit(parent),
    m_charWidths{256}     {
    setWordWrapMode(QTextOption::WrapAnywhere);

    auto *doc     = new QTextDocument(this);
    auto *tempDoc = doc->clone(this);

    setDocument(doc);
    m_tempDoc = tempDoc;
    m_tempDoc->setObjectName("m_tempDoc");

    /*
       QTextMarkdownWriter doesn't output other formattings with monospaced fonts.
     */
    QFont newFont(QStringLiteral("monospace"));
    newFont.setFixedPitch(true);
    newFont.setStyleHint(QFont::Monospace);
    newFont.setPointSize(10);
    document()->setDefaultFont(newFont);
    setFont(newFont);

    QFont curFont = font();
    initSimilarWidthCharGroups(curFont, false);
    curFont.setBold(true);
    initSimilarWidthCharGroups(curFont, true);

    QTextCharFormat fmt = currentCharFormat();
    fmt.setProperty(BorderProperty, QPen(Qt::red));
    setCurrentCharFormat(std::move(fmt));

    registerInterface<TranslateTextObjectInterface>(TextObject::Translate);
    registerInterface<ScoreboardTextObjectInterface>(TextObject::Scoreboard);
    registerInterface<KeybindTextObjectInterface>(TextObject::Keybind);

    m_updateRegionsTimer->setSingleShot(true);
    m_updateRegionsTimer->setInterval(0);
    connect(m_updateRegionsTimer, &QTimer::timeout,
            this, &RawJsonTextEdit::updateFragmentRegions);

    connect(m_obfuscatedTimer, &QTimer::timeout, this,
            &RawJsonTextEdit::updateTempDoc);
    m_obfuscatedTimer->start(200);

    m_blinkCursorTimer->setInterval(500);
    connect(m_blinkCursorTimer, &QTimer::timeout, this, [this](){
        m_drawTextCursor = !m_drawTextCursor;
        update(cursorRect());
    });

    connect(this, &QTextEdit::cursorPositionChanged,
            this, &RawJsonTextEdit::onCursorPositionChanged);

    connect(document()->documentLayout(),
            &QAbstractTextDocumentLayout::updateBlock,
            this, &RawJsonTextEdit::scheduleRegionsUpdate);
    connect(document(), &QTextDocument::contentsChange,
            this, &RawJsonTextEdit::scheduleRegionsUpdate);
    updateFragmentRegions();
}

bool RawJsonTextEdit::joinBorders() const {
    return m_joinBorders;
}

void RawJsonTextEdit::setJoinBorders(bool newJoinBorders) {
    if (newJoinBorders != m_joinBorders) {
        m_joinBorders = newJoinBorders;
        updateFragmentRegions();
    }
}

void RawJsonTextEdit::paintEvent(QPaintEvent *event) {
    ensurePolished();

    QPainter      qp(viewport());
    const QPointF offset(-horizontalScrollBar()->value(),
                         -verticalScrollBar()->value());
    const auto &rect = QRectF(event->rect()).translated(-offset);

    qp.translate(offset);
    qp.setRenderHint(QPainter::Antialiasing);

    if (!m_borderPaths.isEmpty()) {
        if (m_borderPaths.last().path.boundingRect().bottom() >= rect.y()) {
            BorderPaths toDraw;
            for (const auto &borderPath: qAsConst(m_borderPaths)) {
                const auto &path = borderPath.path;
                if (!path.intersects(rect)) {
                    if (path.boundingRect().top() > rect.bottom()) {
                        break;
                    }
                    continue;
                }
                toDraw.append(borderPath);
            }
            if (!toDraw.isEmpty()) {
                for (const auto &borderPath: toDraw) {
                    qp.setPen(borderPath.pen);
                    qp.drawPath(borderPath.path);
                }
            }
        }
    }

    QAbstractTextDocumentLayout::Selection selection;
    const auto                           &&cursor = textCursor();
    selection.cursor = cursor;

    QTextCharFormat fmt;
    fmt.setForeground(palette().highlightedText());
    fmt.setBackground(palette().highlight());
    selection.format = fmt;

    QAbstractTextDocumentLayout::PaintContext ctx;
    if (m_drawTextCursor && hasFocus())
        ctx.cursorPosition = cursor.position();
    ctx.selections = { selection };
//    ctx.clip       = rect;

    qp.save();
    qp.setRenderHint(QPainter::TextAntialiasing);
//    qp.setClipRect(rect, Qt::IntersectClip);

    if (m_renderingDoc) {
        m_renderingDoc->documentLayout()->draw(&qp, ctx);
    }

    qp.restore();
    qp.end();

    //    QTextEdit::paintEvent(event);

    //    {
    //        QPainter      p(viewport());
    //        const QPointF offset(
    //            -horizontalScrollBar()->value(),
    //            -verticalScrollBar()->value());
    //        const auto &eventRect = QRectF(event->rect()).translated(-offset);
    //        p.setRenderHint(QPainter::Antialiasing);
    //        p.translate(offset);
    //        for (const auto &fragRegion: qAsConst(m_fragRegions)) {
    //            int i = 0;
    //            for (const auto &subfragment: fragRegion.subfragments) {
    //                const auto &rect = subfragment.rect;
    //                const auto &text = subfragment.text;
    ////                qDebug() << rect << text;
    //                if (eventRect.intersects(rect)) {
    //                    p.drawRoundedRect(rect, 5, 5);
    //                    p.drawText(rect, text);
    //                    ++i;
    //                } else if (rect.top() > eventRect.bottom()) {
    //                    goto BREAK_FRAG_REGIONS;
    //                }
    //            }
    //        }
    //    }
    // BREAK_FRAG_REGIONS:
    //    return;
}

void RawJsonTextEdit::resizeEvent(QResizeEvent *event) {
    QTextEdit::resizeEvent(event);
    scheduleRegionsUpdate();
}

void RawJsonTextEdit::focusInEvent(QFocusEvent *event) {
    ensureCursorVisible();
    QTextEdit::focusInEvent(event);
}

void RawJsonTextEdit::focusOutEvent(QFocusEvent *event) {
    m_blinkCursorTimer->stop();
    QTextEdit::focusOutEvent(event);
}

QMimeData * RawJsonTextEdit::createMimeDataFromSelection() const {
    auto *mime = QTextEdit::createMimeDataFromSelection();

    const auto &&cursor   = textCursor();
    const int    selStart = cursor.selectionStart();
    const int    selEnd   = qMax(0, cursor.selectionEnd() - 1);

    QVector<QTextLayout::FormatRange> ranges;

    for (const auto &fragRegion: m_fragRegions) {
        const auto &fragment = fragRegion.fragment;
        if (!fragment.isValid()) {
            continue;
        }
        const int fragStart    = fragment.position();
        const int fragEnd      = fragStart + fragment.length() - 1;
        const int overlapStart = qMax(selStart, fragStart);
        const int overlapEnd   = qMin(selEnd, fragEnd);
        if (overlapStart <= overlapEnd) {
            const int   pos    = overlapStart - selStart;
            const int   length = overlapEnd - overlapStart + 1;
            const auto &format = fragment.charFormat();

            if (format.boolProperty(TextObfuscated)) {
                ranges.append({ pos, length, format });
            }
        }
    }

    QByteArray  bytes;
    QDataStream ds(&bytes, QIODevice::WriteOnly);
    ds << ranges;

    mime->setData("application/octet-stream", bytes);

    return mime;
}

void RawJsonTextEdit::insertFromMimeData(const QMimeData *source) {
    auto    &&cursor   = textCursor();
    const int selStart = cursor.selectionStart();

    QTextEdit::insertFromMimeData(source);

    QVector<QTextLayout::FormatRange> ranges;

    const QByteArray &&bytes = source->data("application/octet-stream");
    QDataStream        ds(bytes);
    ds >> ranges;

    cursor.joinPreviousEditBlock();

    for (const auto &range: qAsConst(ranges)) {
        cursor.setPosition(selStart + range.start);
        cursor.setPosition(selStart + range.start + range.length,
                           QTextCursor::KeepAnchor);
        cursor.setCharFormat(range.format);
    }

    cursor.setPosition(selStart);
    cursor.endEditBlock();
    setTextCursor(cursor);
}

void RawJsonTextEdit::mouseReleaseEvent(QMouseEvent *event) {
    QTextEdit::mouseReleaseEvent(event);

    const QPoint &&eventPos = event->pos();
    QTextCursor  &&cursor   = cursorForPosition(eventPos);

    const QRect &&rect = cursorRect(cursor);
    if (rect.x() > eventPos.x()) {
        cursor.movePosition(QTextCursor::Left);
    }

    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
    const auto &selection = cursor.selectedText();
    if ((!selection.isEmpty()) &&
        (selection[0] == QChar::ObjectReplacementCharacter)) {
        setTextCursor(cursor);
    }
}

void RawJsonTextEdit::mouseDoubleClickEvent(QMouseEvent *event) {
    QTextCursor &&cursor    = textCursor();
    const auto   &selection = cursor.selectedText();

    if ((selection.length() == 1) &&
        (selection[0] == QChar::ObjectReplacementCharacter)) {
        auto    &&fmt     = cursor.charFormat();
        const int objType = fmt.objectType();

        if (objType > TextObject::_begin && objType < TextObject::_end) {
            const auto *handler = static_cast<RawJsonTextObjectInterface *>(
                document()->documentLayout()->handlerForObject(fmt.objectType()));

            const auto &&bottomRight = cursorRect(cursor).bottomRight();
            cursor.setPosition(cursor.anchor());
            const auto &&topLeft = cursorRect(cursor).topLeft();
            const QRect  objRect{ topLeft, bottomRight };

            if (handler && handler->editObject(fmt, objRect)) {
                setCurrentCharFormat(fmt);
            }
            event->accept();
        } else {
            QTextEdit::mouseDoubleClickEvent(event);
        }
    } else {
        QTextEdit::mouseDoubleClickEvent(event);
    }
}

bool RawJsonTextEdit::renderObfuscation() const {
    return m_renderObfuscation;
}

void RawJsonTextEdit::setRenderObfuscation(bool newRenderObfuscation) {
    m_renderObfuscation = newRenderObfuscation;
    if (newRenderObfuscation) {
        m_obfuscatedTimer->start();
    } else {
        m_obfuscatedTimer->stop();
    }
    viewport()->update();
}

void RawJsonTextEdit::initSimilarWidthCharGroups(const QFont &font,
                                                 bool isBold) {
    struct CharWidth {
        QChar ch;
        int   width;
    };

    QVector<CharWidth> charWidths;
    const QFontMetrics metrics(font);
    const int          mask = isBold * boldCharMask;

    for (const auto &ch: m_obfuscatedCharset) {
        if (metrics.inFont(ch)) {
            const int width = metrics.horizontalAdvance(ch);
            //            m_charWidths.emplace(ch, width);
            charWidths.append({ ch, width });
        }
    }

    std::sort(charWidths.begin(), charWidths.end(),
              [](const CharWidth &o1, const CharWidth &o2) {
        return o1.width < o2.width;
    });

    int     lastWidth = charWidths.first().width;
    QString charGroup;
    for (const auto &charWidth: charWidths) {
        if (charWidth.width != lastWidth) {
            m_similarWidthCharGroups[lastWidth | mask] = charGroup;
            charGroup.clear();
            lastWidth = charWidth.width;
        }
        charGroup.append(charWidth.ch);
    }
}

void RawJsonTextEdit::scheduleRegionsUpdate() {
    ensureCursorVisible();
    m_updateRegionsTimer->start();
}

// Adapted from: https://stackoverflow.com/a/72746121/12682038
void RawJsonTextEdit::updateFragmentRegions() {
    if (toPlainText().isEmpty()) {
        if (!m_fragRegions.isEmpty()) {
            m_fragRegions.clear();
            viewport()->update();
        }
        if (!m_borderPaths.isEmpty()) {
            m_borderPaths.clear();
            viewport()->update();
        }
        return;
    }
    const auto *doc       = document();
    auto        block     = doc->begin();
    const auto &end       = doc->end();
    const auto *docLayout = doc->documentLayout();

    FragmentRegions fragRegions;
    Subfragments    lastSubfragments;
    QTextFragment   lastFragment;
    while (block != end) {
        if (block.text().isEmpty()) {
            block = block.next();
            continue;
        }

        const auto &blockRect = docLayout->blockBoundingRect(block);
        const qreal blockX    = blockRect.x();
        const qreal blockY    = blockRect.y();

        for (auto it = block.begin(); !it.atEnd(); ++it) {
            const auto &fragment = it.fragment();
            if (!fragment.isValid()) {
                continue;
            }
            const auto &fmt = fragment.charFormat();

            if (!lastSubfragments.isEmpty()) {
                fragRegions.append({ lastFragment, lastSubfragments });
                lastSubfragments.clear();
            }

            if ((fmt.property(BorderProperty).type() == QVariant::Pen)
                || (fmt.boolProperty(TextObfuscated))) {
                const auto *blockLayout = block.layout();
                int         fragPos     = fragment.position() -
                                          block.position();
                const int       fragEnd      = fragPos + fragment.length();
                const QString &&text         = fragment.text();
                const int       fragRelStart = fragment.position() -
                                               block.position();
                while (true) {
                    const auto &line = blockLayout->lineForTextPosition(
                        fragPos);
                    if (line.isValid()) {
                        qreal  x       = line.cursorToX(fragPos);
                        int    lineEnd = fragEnd;
                        qreal  right   = line.cursorToX(&lineEnd);
                        QRectF rect(
                            blockX + x, blockY + line.y(),
                            right - x, line.height());

                        const int textPos = qMax(0,
                                                 line.textStart() -
                                                 fragRelStart);
                        const auto &&subtext =
                            text.mid(textPos, qMin(line.textLength(),
                                                   fragment.length() -
                                                   textPos));
                        lastSubfragments.append({ rect, subtext });
                        if (lineEnd != fragEnd) {
                            fragPos = lineEnd;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
            }
            lastFragment = fragment;
        }
        block = block.next();
    }

    if (!lastSubfragments.isEmpty() && lastFragment.isValid()) {
        fragRegions.append({ lastFragment, lastSubfragments });
    }

    m_fragRegions = fragRegions;

    BorderPaths borderPaths;
    if (!m_joinBorders) {
        for (const auto &fragRegion: qAsConst(fragRegions)) {
            QPainterPath path;
            for (const auto &subfragment: fragRegion.subfragments) {
                path.addRect(subfragment.rect.adjusted(0, 0, -1, -1));
            }
            path.translate(.5, .5);
            borderPaths.append({ path, fragRegion.formatProperty(
                                     BorderProperty).value<QPen>() });
        }
    } else {
        for (const auto &fragRegion: qAsConst(fragRegions)) {
            QPainterPath path;
            for (const auto &subfragment: fragRegion.subfragments) {
                path.addRect(subfragment.rect);
            }
            path.translate(.5, .5);
            path = path.simplified();
            QPainterPath                         fixPath;
            std::optional<QPainterPath::Element> last;
            // Shrink each rectangle to avoid overlapping
            for (int i = 0; i < path.elementCount(); ++i) {
                auto element = path.elementAt(i);
                if (element.type != QPainterPath::MoveToElement) {
                    if (element.x < last->x) {
                        last->y   -= 1;
                        element.y -= 1;
                    } else if (element.y > last->y) {
                        last->x   -= 1;
                        element.x -= 1;
                    }
                }
                if (last) {
                    if (last->isMoveTo()) {
                        fixPath.moveTo(last->x, last->y);
                    } else{
                        fixPath.lineTo(last->x, last->y);
                    }
                }
                last = element;
            }
            if (last->isLineTo()) {
                fixPath.lineTo(last->x, last->y);
            }
            borderPaths.append({ fixPath,
                                 fragRegion.formatProperty(
                                     BorderProperty).value<QPen>() });
        }
    }
    m_borderPaths = borderPaths;

    if (m_renderObfuscation) {
        m_renderingDoc = document();
        m_obfuscatedTimer->start();
    }

    viewport()->update();
}

void RawJsonTextEdit::obfuscateString(QString &string,
                                      const bool isBold,
                                      const QFontMetrics &metrics) {
    if (!m_renderObfuscation)
        return;

    int i = 0;
    for (const auto &ch: string) {
        int              width = INT_MIN;
        const RenderChar key{ ch, isBold };
        if (m_charWidths.contains(key)) {
            width = m_charWidths.lookup(key);
        } else {
            width = metrics.horizontalAdvance(ch);
            m_charWidths.emplace(key, width | (boldCharMask * isBold));
        }
        if (m_similarWidthCharGroups.contains(width)) {
            string[i] = Glhp::randChr(
                m_similarWidthCharGroups[width]);
        }

        ++i;
    }
}

void RawJsonTextEdit::updateTempDoc() {
    if (!isVisible()) {
        return;
    }
    Q_ASSERT(m_tempDoc != nullptr);

    m_tempDoc->clear();
    m_tempDoc->setDefaultFont(document()->defaultFont());
    m_tempDoc->setDefaultTextOption(document()->defaultTextOption());
    m_tempDoc->setTextWidth(document()->textWidth());

    auto &&cursor = textCursor();
    cursor.select(QTextCursor::Document);
    const auto &&selection = cursor.selection();

    QTextCursor tempCursor(m_tempDoc);
    tempCursor.beginEditBlock();
    tempCursor.insertFragment(selection);

    for (const auto &fragRegion: qAsConst(m_fragRegions)) {
        const auto &fragment = fragRegion.fragment;
        if (!fragment.charFormat().boolProperty(TextObfuscated)) {
            continue;
        }
        const auto       &&font = fragment.charFormat().font();
        const QFontMetrics metrics(font);
        const bool         isBold = font.bold();
        tempCursor.setPosition(fragment.position());
        tempCursor.setPosition(fragment.position() + fragment.length(),
                               QTextCursor::KeepAnchor);
        const QString &&fragText = fragment.text();
        if (fragText == QChar::ObjectReplacementCharacter) {
            continue;
        }
        QString obfuscated = fragText;
        obfuscateString(obfuscated, isBold, metrics);
        tempCursor.insertText(obfuscated);
    }
    tempCursor.endEditBlock();

    m_renderingDoc = m_tempDoc;
    viewport()->update();
}

void RawJsonTextEdit::onCursorPositionChanged() {
    ensureCursorVisible();
}

void RawJsonTextEdit::ensureCursorVisible() {
    m_blinkCursorTimer->start();
    m_drawTextCursor = true;
    viewport()->update(cursorRect());
}
