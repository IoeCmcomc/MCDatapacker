#include "parser.h"

Command::Parser::ParsingError::ParsingError(const QString &what_arg)
    : runtime_error(what_arg.toStdString()) {
    message = what_arg;
}

void Command::Parser::error(QStringView msg) {
    QString errorIndicatorText = '"' + m_text.left(m_pos - 1) + "«" +
                                 m_curChar + "»" + m_text.midRef(m_pos + 1) +
                                 "\" (" + m_text.length() +
                                 " chars)";

    throw Command::Parser::ParsingError(tr(
                                            "Input: %1\nCommand::Parser error at pos %2: %3").arg(
                                            errorIndicatorText,
                                            QString
                                            ::number(
                                                m_pos), msg));
}

/*!
 * \brief Advances \a n characters, then set the current character to \c m_curChar.
 */
void Command::Parser::advance(int n) {
    if (n < 1)
        return;

    for (int i = 0; i < n; i++) {
        m_pos++;
        if (m_pos >= m_text.length())
            m_curChar = QChar();
        else
            m_curChar = m_text[m_pos];
    }
}

/*!
 * \brief recedes \a n characters, then set the current character to \c m_curChar.
 */
void Command::Parser::recede(int n) {
    if (n < 1)
        return;

    for (int i = 0; i < n; i++) {
        if (m_pos > 0)
            m_pos--;
        if (m_pos >= m_text.length())
            m_curChar = QChar();
        else
            m_curChar = m_text[m_pos];
    }
}

/*!
 * \brief throws a \c CommandParsingError if the char \a chr isn't equal to the current character.
 */
bool Command::Parser::expect(const QChar &chr, bool acceptNull) {
    bool cond = m_curChar == chr;

    if (!cond && acceptNull)
        cond = m_curChar.isNull();
    if (cond) {
        return true;
    } else {
        QString curCharTxt = (m_curChar.isNull()) ? "end of line" : QString(
            "character '%1'").arg(m_curChar);
        QString charTxt = (chr.isNull()) ? "end of line" : QString(
            "character '%1'").arg(chr);
        if (acceptNull)
            error(QString("Unexpected %1, expecting %2 or end of line").arg(
                      m_curChar, charTxt));
        else
            error(QString("Unexpected %1, expecting %2").arg(m_curChar,
                                                             charTxt));
        return false;
    }
}

/*!
 * \brief Much like \c expect(), but also advances one character.
 * \sa eat()
 */
void Command::Parser::eat(const QChar &chr, bool acceptNull) {
    expect(chr, acceptNull);
    advance();
}

/*!
 * \brief Returns the substring from the current character until it meets the character \a chr (exclusive).
 */
QString Command::Parser::getUntil(const QChar &chr) {
    QString ret;

    while (m_curChar != chr) {
        if (m_curChar.isNull())
            break;
        ret += m_curChar;
        advance();
    }
    return ret;
}

/*!
 * \brief Returns the substring from the current character with the given (regex) \a charset.
 */
QString Command::Parser::getWithCharset(const QString &charset) {
    QString            ret;
    QRegularExpression regex("[" + charset + "]");

    while (regex.match(m_curChar).hasMatch()) {
        if (m_curChar.isNull())
            break;
        ret += m_curChar;
        advance();
    }
    return ret;
}

QString Command::Parser::peek(int n) {
    return m_text.mid(m_pos, n);
}

void Command::Parser::skipWs() {
    if (m_curChar.isNull())
        advance();
}

/*!
 * \brief Returns the next literal string (word) without advancing the current pos.
 */
QString Command::Parser::peekLiteral() {
    int     pos     = m_pos;
    QChar   curChar = m_curChar;
    QString ret;

    while ((curChar != " ") && (pos < m_text.length())) {
        if (m_curChar.isNull())
            break;
        ret += m_curChar;
        pos++;
        curChar = m_text[pos];
    }
    return ret;
}
