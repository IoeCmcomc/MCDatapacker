#include "rawjsontextobjectinterface.h"

#include "rawjsontextedit.h"
#include "translatedtextobjectdialog.h"
#include "scoreboardtextobjectdialog.h"
#include "entitynamestextobjectdialog.h"
#include "nbttextobjectdialog.h"

#include <QFontMetrics>
#include <QPainter>
#include <QLineEdit>
#include <QDebug>
#include <QEventLoop>
#include <QJsonObject>

RawJsonTextObjectInterface::RawJsonTextObjectInterface(
    RawJsonTextEdit *textEdit, const QString &iconStr)
    : QObject(textEdit), m_iconStr(iconStr), m_textEdit(textEdit) {
    Q_ASSERT(textEdit != nullptr);
}

QSizeF RawJsonTextObjectInterface::intrinsicSize(
    [[maybe_unused]] QTextDocument *doc, [[maybe_unused]] int posInDocument,
    const QTextFormat &format) {
    const QTextCharFormat &charFormat = format.toCharFormat();
    const QString        &&text       = displayText(format);

    QFontMetrics metrics(charFormat.font());
    QSizeF       size(metrics.horizontalAdvance(text), metrics.height());

    QFont iconFont(QStringLiteral("MCDatapacker-Icons"),
                   charFormat.font().pointSize());
    QFontMetrics iconMetrics{ iconFont };
    QRect        iconRect = iconMetrics.boundingRect(m_iconStr);

    size.rwidth() += iconRect.width();

    return size;
}

void RawJsonTextObjectInterface::drawObject(
    QPainter *painter, const QRectF &rect, [[maybe_unused]] QTextDocument *doc,
    [[maybe_unused]] int posInDocument, const QTextFormat &format) {
    const QTextCharFormat &charFmt = format.toCharFormat();
    const auto            &font    = charFmt.font();

    QFont iconFont(QStringLiteral("MCDatapacker-Icons"),
                   charFmt.font().pointSize());
    QFontMetrics iconMetrics{ iconFont };
    QRect        iconRect = iconMetrics.boundingRect(m_iconStr);

    iconRect.moveTopLeft({ 0, 0 });

    QPixmap pixmap{ iconRect.size() };
    pixmap.fill(Qt::transparent);
    {
        QPainter p(&pixmap);
        p.setPen(m_textEdit->palette().midlight().color());
        p.setOpacity(0.7);
        p.setFont(iconFont);
        p.drawText(iconRect, m_iconStr);
    }

    QBrush     iconBg(pixmap);
    QTransform iconTrans;

    iconTrans.rotate(45);
    iconBg.setTransform(iconTrans);

    painter->save();
    painter->fillRect(rect, iconBg);

    const QRectF iconRect2(rect.topLeft(), iconRect.size());
    painter->setFont(iconFont);
    painter->setPen(m_textEdit->palette().text().color());
    painter->drawText(iconRect2, m_iconStr);
    painter->restore();

    const QRectF &textRect = rect.adjusted(iconRect2.width(), 0, 0, 0);
    QString     &&text     = displayText(format);
    if (format.boolProperty(RawJsonProperty::TextObfuscated)) {
        m_textEdit->obfuscateString(text, font.bold(), QFontMetrics(font));
    }

    //    painter->drawText(textRect, text);

    QTextLayout layout(text, charFmt.font());
    layout.setFormats({ { 0, text.length(), charFmt } });
    layout.beginLayout();
    QTextLine line = layout.createLine();
    line.setLineWidth(textRect.width());
    line.setPosition({ 0, 0 });
    layout.endLayout();

    layout.draw(painter, textRect.topLeft());
}

bool RawJsonTextObjectInterface::editObject(
    [[maybe_unused]] QTextFormat &format, [[maybe_unused]] const QRectF &rect) {
    return false;
}


bool RawJsonTextObjectInterface::execEditorDialog(QTextFormat &format,
                                                  AbstractTextObjectDialog *dialog)
{
    QScopedPointer<AbstractTextObjectDialog,
                   QScopedPointerDeleteLater> pointer{ dialog };

    dialog->fromTextFormat(format);

    if (dialog->exec() == QDialog::Accepted) {
        format = dialog->toTextFormat();
        return true;
    } else {
        return false;
    }
}

TranslatedTextObjectInterface::TranslatedTextObjectInterface(
    RawJsonTextEdit *textEdit)
    : RawJsonTextObjectInterface(textEdit, QStringLiteral("translate")) {
}

QString TranslatedTextObjectInterface::displayText(const QTextFormat &format)
const {
    return format.stringProperty(RawJsonProperty::TranslateKey);
}

bool TranslatedTextObjectInterface::editObject(QTextFormat &format,
                                               const QRectF &rect) {
    return execEditorDialog(format, new TranslatedTextObjectDialog(m_textEdit));
}


ScoreboardTextObjectInterface::ScoreboardTextObjectInterface(
    RawJsonTextEdit *textEdit)
    : RawJsonTextObjectInterface(textEdit, QStringLiteral("score")) {
}

QString ScoreboardTextObjectInterface::displayText(const QTextFormat &format)
const {
    if (format.hasProperty(RawJsonProperty::ScoreboardValue)) {
        return format.stringProperty(RawJsonProperty::ScoreboardValue);
    }
    return format.stringProperty(RawJsonProperty::ScoreboardObjective);
}

bool ScoreboardTextObjectInterface::editObject(QTextFormat &format,
                                               const QRectF &rect) {
    return execEditorDialog(format, new ScoreboardTextObjectDialog(m_textEdit));
}


EntityNamesTextObjectInterface::EntityNamesTextObjectInterface(
    RawJsonTextEdit *textEdit)
    : RawJsonTextObjectInterface(textEdit, QStringLiteral("entity")) {
}

QString EntityNamesTextObjectInterface::displayText(const QTextFormat &format)
const {
    return format.stringProperty(RawJsonProperty::Selector);
}

bool EntityNamesTextObjectInterface::editObject(QTextFormat &format,
                                                const QRectF &rect) {
    return execEditorDialog(format,
                            new EntityNamesTextObjectDialog(m_textEdit));
}


KeybindTextObjectInterface::KeybindTextObjectInterface(
    RawJsonTextEdit *textEdit)
    : RawJsonTextObjectInterface(textEdit, QStringLiteral("keyboard")) {
}

QString KeybindTextObjectInterface::displayText(const QTextFormat &format)
const {
    return format.stringProperty(RawJsonProperty::Keybind);
}

bool KeybindTextObjectInterface::editObject(QTextFormat &format,
                                            const QRectF &rect) {
    QLineEdit *edit = new QLineEdit(m_textEdit);

    edit->setWindowFlags(Qt::ToolTip | Qt::Window);
    edit->setAttribute(Qt::WA_DeleteOnClose);

    bool textChanged = false;
    edit->setText(displayText(format));
    connect(edit, &QLineEdit::textEdited, edit, [&textChanged](){
        textChanged = true;
    });

    QRect displayRect{ m_textEdit->mapToGlobal(rect.topLeft().toPoint()),
                       rect.size().toSize() };
    const QRect editorRect{ m_textEdit->mapToGlobal({ 0, 0 }),
                            m_textEdit->size() };
    if (displayRect.left() < editorRect.left()) {
        displayRect.setLeft(editorRect.left());
    }
    if (displayRect.right() > editorRect.right()) {
        displayRect.setRight(editorRect.right());
    }

    edit->setGeometry(displayRect);
    edit->show();
    edit->activateWindow();
    edit->grabKeyboard();
    edit->setFocus(Qt::PopupFocusReason);

    QEventLoop loop;
    connect(edit, &QLineEdit::editingFinished, &loop, &QEventLoop::quit);
    loop.exec();

    if (textChanged) {
        format.setProperty(RawJsonProperty::Keybind, edit->text());
    }
    edit->close();
    return textChanged;
}


NbtTextObjectInterface::NbtTextObjectInterface(RawJsonTextEdit *textEdit)
    : RawJsonTextObjectInterface(textEdit, QStringLiteral("snbt")) {
}

QString NbtTextObjectInterface::displayText(const QTextFormat &format) const {
    return format.stringProperty(RawJsonProperty::NbtPath);
}

bool NbtTextObjectInterface::editObject(QTextFormat &format,
                                        const QRectF &rect) {
    return execEditorDialog(format, new NbtTextObjectDialog(m_textEdit));
}
