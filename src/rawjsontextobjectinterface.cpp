#include "rawjsontextobjectinterface.h"

#include "rawjsontextedit.h"

#include <QFontMetrics>
#include <QPainter>

RawJsonTextObjectInterface::RawJsonTextObjectInterface(
    RawJsonTextEdit *textEdit, const QString &iconStr)
    : QObject(textEdit), m_iconStr(iconStr), m_textEdit(textEdit) {
}

QSizeF RawJsonTextObjectInterface::intrinsicSize(QTextDocument *doc,
                                                 int posInDocument,
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
    QPainter *painter, const QRectF &rect, QTextDocument *doc,
    int posInDocument, const QTextFormat &format) {
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

TranslateTextObjectInterface::TranslateTextObjectInterface(
    RawJsonTextEdit *textEdit)
    : RawJsonTextObjectInterface(textEdit, QStringLiteral("translate")) {
}

QString TranslateTextObjectInterface::displayText(const QTextFormat &format)
const {
    return format.stringProperty(RawJsonProperty::TranslateKey);
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

KeybindTextObjectInterface::KeybindTextObjectInterface(
    RawJsonTextEdit *textEdit)
    : RawJsonTextObjectInterface(textEdit, QStringLiteral("keyboard")) {
}

QString KeybindTextObjectInterface::displayText(const QTextFormat &format)
const {
    return format.stringProperty(RawJsonProperty::Keybind);
}
