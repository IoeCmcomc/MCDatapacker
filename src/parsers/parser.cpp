#include "parser.h"

#include <QCoreApplication>
#include <QRegularExpression>

Parser::Error::Error(const QString &whatArg, int pos, int length,
                     const QVariantList &args)
    : std::runtime_error(whatArg.toStdString()), pos(pos), length(length), args(
        args) {
}

QString Parser::Error::toLocalizedMessage() const {
    QString &&errMsg =
        QCoreApplication::translate("Parser::Error", what());

    if (errMsg == what())
        errMsg = QCoreApplication::translate("Parser", what());

    for (int i = 0; i < args.size(); ++i) {
        errMsg = errMsg.arg(args.at(i).toString());
    }

    const QString &&ret =
        QCoreApplication::translate("Parser", "Syntax error at position %1: %2")
        .arg(pos).arg(errMsg);
    return std::move(ret);
}

bool Parser::Error::operator==(const Error &o) const {
    return std::make_tuple(what(), pos, length) == std::make_tuple(
        o.what(), o.pos, o.length);
}

Parser::Parser() {
}

Parser::Parser(const QString &input) : m_text(input) {
}

int Parser::pos() const {
    return m_pos;
}

/*!
 * \brief Sets the current position to parse to \a pos
 * and updates the current character.
 */
void Parser::setPos(int pos) {
    m_pos = pos;
    if (pos < 0)
        qWarning("The current position will be less than 0");
    if (pos >= m_text.length())
        m_curChar = QChar();
    else
        m_curChar = m_text.at(pos);
}

QChar Parser::curChar() const {
    return m_curChar;
}

/*!
 * \brief Returns the text which is parsed.
 */
QString Parser::text() const {
    return m_text;
}

/*!
 * \brief Sets the text which is parsed and resets the current position.
 */
void Parser::setText(const QString &text) {
    m_text = text;
    setPos(0);
}

Parser::Errors Parser::errors() const {
    return m_errors;
}

QStringSet Parser::spans() const {
    return m_spans;
}

/*!
 * \brief Throws a \c Command::Parser::ParsingError with a formatted message.
 */
void Parser::error(const QString &msg, const QVariantList &args) {
    error(msg, args, m_pos);
}

void Parser::error(const QString &msg, const QVariantList &args,
                   int pos, int length) {
    /*qWarning() << "Command::Parser::error" << msg << pos << length; */
/*
      QString errorIndicatorText = QString("\"%1«%2»%3\" (%4 chars)").arg(
          m_text.mid(pos - 10, 10),
          m_text.mid(pos, length),
          m_text.mid(pos + length, 10),
          QString::number(m_text.length()));
 */

    throw Parser::Error(msg, pos, length, args);
}

/*!
 * \brief Advances \a n characters, then updates the current character.
 */
void Parser::advance(int n) {
    if (n < 1)
        return;

    setPos(m_pos + n);
}

/*!
 * \brief throws a \c error if the char \a chr isn't equal to the current character.
 */
bool Parser::expect(const QChar &chr, bool acceptNull) {
/*    qDebug() << "Command::Parser::expect" << chr << acceptNull << m_curChar; */
    bool cond = m_curChar == chr;

    if (!cond && acceptNull)
        cond = m_curChar.isNull();
    if (cond) {
        return true;
    } else {
        const QString &&curCharTxt =
            (m_curChar.isNull()) ? QStringLiteral("EOL") : QLatin1Char('\'')
            +
            m_curChar +
            QLatin1Char('\'');
        const QString &&charTxt =
            (chr.isNull()) ? QStringLiteral("EOL") : QLatin1Char('\'') +
            chr +
            QLatin1Char('\'');
        error(QT_TR_NOOP("Unexpected %1, expecting %2"),
              { curCharTxt, charTxt }, m_pos, 1);
        return false;
    }
}

/*!
 * \brief Much like \c expect(), but also advances one character.
 * \sa eat()
 */
QString Parser::eat(const QChar &chr, bool acceptNull) {
    expect(chr, acceptNull);
    advance();
    return spanText(m_pos - 1);
}

/*!
 * \brief Returns the substring from the current character until it meets the character \a chr (exclusive).
 */
QStringRef Parser::getUntil(QChar chr) {
    const QStringRef &ref = peekUntil(chr);

    advance(ref.length());
    return ref;
}

/*!
 * \brief Returns the remaining substring from the current character.
 */
QStringRef Parser::getRest() {
    const auto &rest = m_text.midRef(m_pos);

    advance(rest.length());
    return rest;
}

/*!
 * \brief Returns the substring from the current character with the given (regex) \a charset.
 */
QString Parser::getWithCharset(const QString &charset) {
    QString            ret;
    QRegularExpression regex('[' + charset + ']');

    while (regex.match(m_curChar).hasMatch()) {
        if (m_curChar.isNull())
            break;
        ret += m_curChar;
        advance();
    }
    return ret;
}

/*!
 * \brief Returns the substring from the current character with the given regex \a pattern.
 */
QString Parser::getWithRegex(const QString &pattern) {
    return getWithRegex(QRegularExpression(pattern));
}

/*!
 * \brief Returns the substring from the current character with the given \a regex.
 */
QString Parser::getWithRegex(const QRegularExpression &regex) {
    QString ret;

    const auto &match = regex.match(m_text, m_pos,
                                    QRegularExpression::NormalMatch,
                                    QRegularExpression::AnchoredMatchOption);

    if (match.hasMatch()) {
        ret = match.captured();
        advance(ret.length());
    }
    return ret;
}

QStringRef Parser::peek(int n) {
    return m_text.midRef(m_pos, n);
}

QStringRef Parser::peekUntil(QChar chr) {
    const int start = m_pos;
    const int index = m_text.indexOf(chr, start);

    const auto &ref = m_text.midRef(start, qMax(-1, index - start));

    return ref;
}

QString Parser::skipWs(bool once) {
    const int start = m_pos;

    while (m_curChar.isSpace()) {
        advance();
        if (once)
            break;
    }
    return spanText(start);
}

/*!
 * \brief Returns the next quoted string.
 */
QString Parser::getQuotedString() {
    QVector<QChar> delimiters{ '"', '\'' };

    QChar curQuoteChar;

    if (delimiters.contains(m_curChar))
        curQuoteChar = m_curChar;
    eat(curQuoteChar);
    QString value;
    bool    backslash = false;
    while ((m_curChar != curQuoteChar) || backslash) {
        if (m_pos >= m_text.length())
            error(QT_TR_NOOP("Incomplete quoted string"));
        if (backslash) {
            if (m_curChar == curQuoteChar) {
                value += curQuoteChar;
            } else {
                switch (m_curChar.toLatin1()) {
                    case '\\': {
                        value += '\\';
                        break;
                    }

                    case 'b': {
                        value += '\b';
                        break;
                    }

                    case 'f': {
                        value += '\f';
                        break;
                    }

                    case 'n': {
                        value += '\n';
                        break;
                    }

                    case 't': {
                        value += '\t';
                        break;
                    }

                    case 'r': {
                        value += '\r';
                        break;
                    }

                    default:
                        value += m_curChar;
                }
            }
            backslash = false;
        } else if (m_curChar == '\\') {
            backslash = true;
        } else {
            value += m_curChar;
        }
        advance();
    }
    eat(curQuoteChar);
    return value;
}

QString Parser::spanText(const QStringRef &textRef) {
    return spanText(textRef.toString());
}

QString Parser::spanText(const QString &text) {
    m_spans << text;
    return *m_spans.find(text);
}

QString Parser::spanText(int start) {
    return spanText(m_text.mid(start, m_pos - start));
}
