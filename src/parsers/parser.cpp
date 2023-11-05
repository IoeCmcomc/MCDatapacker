#include "parser.h"

#include <QCoreApplication>
#include <QRegularExpression>

Parser::Error::Error(const QString &whatArg, int pos, int length,
                     const QVariantList &args)
    : std::runtime_error(whatArg.toStdString()), pos(pos), length(length), args(
        args) {
}

Parser::Error::Error(const char *whatArg, int pos, int length,
                     const QVariantList &args)
    : std::runtime_error(whatArg), pos(pos), length(length), args(args) {
}

QString Parser::Error::toLocalizedMessage() const {
    QString &&errMsg = tr(what());

    for (int i = 0; i < args.size(); ++i) {
        errMsg = errMsg.arg(args.at(i).toString());
    }

    const QString &&ret = tr("Syntax error at position %1: %2")
                          .arg(pos).arg(errMsg);
    return std::move(ret);
}

bool Parser::Error::operator==(const Error &o) const {
    return std::tie(pos, length) == std::tie(o.pos, o.length);
}

Parser::Parser() {
}

Parser::Parser(const QString &text) : m_srcText(text) {
    m_text = m_srcText;
    setPos(0);
}

Parser::Parser(QString &&text) : m_srcText(text) {
    m_text = m_srcText;
    setPos(0);
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
    if (!m_srcText.isNull()) {
        return m_srcText;
    } else {
        return m_text.toString();
    }
}

QStringView Parser::textView() const {
    return m_text;
}

/*!
 * \brief Sets the text which is parsed and resets the current position.
 */
void Parser::setText(const QString &text) {
    m_srcText = text;
    m_text    = m_srcText;
    setPos(0);
}

void Parser::setText(QString &&newText) {
    m_srcText = std::move(newText);
    m_text    = m_srcText;
    setPos(0);
}

void Parser::setText(QStringView text) {
    m_text = text;
}

Parser::Errors Parser::errors() const {
    return m_errors;
}

StringHash Parser::spans() const {
    return m_spans;
}

/*!
 * \brief Throws a \c Command::Parser::ParsingError with a formatted message.
 */
void Parser::throwError(const QString &msg, const QVariantList &args) {
    throwError(msg, args, m_pos);
}

void Parser::throwError(const char *msg, const QVariantList &args) {
    throwError(msg, args, m_pos);
}

/*!
 * \brief Throws a \c Command::Parser::ParsingError with a formatted message,
 * a absoulte position and a length.
 */
void Parser::throwError(const QString &msg, const QVariantList &args,
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

void Parser::throwError(const char *msg, const QVariantList &args,
                        int pos, int length) {
    throw Parser::Error(msg, pos, length, args);
}

/*!
 * \brief Adds a \c Command::Parser::ParsingError with a formatted message
 * to the error list and continue.
 */
void Parser::reportError(const char *msg, const QVariantList &args) {
    reportError(msg, args, pos());
}

/*!
 * \brief Adds a \c Command::Parser::ParsingError with a formatted message
 * to the error list and continue.
 */
void Parser::reportError(const char *msg, const QVariantList &args,
                         int pos, int length) {
    m_errors << Parser::Error(msg, pos, length, args);
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
 * \brief Advances the current position using the string view \a sv.
 */
QStringView Parser::advanceView(QStringView sv) {
    advance(sv.length());
    return sv;
}

/*!
 * \brief Throws a \c error if the char \a chr isn't equal to the current character.
 * If \c errMsg is not specified, the error message will have the form
 * "Unexpected %1, expecting %2".
 *
 * \sa eat()
 */
bool Parser::expect(QChar chr, const char *errMsg) {
    if (m_curChar == chr) {
        return true;
    } else {
        const QString &&curCharTxt =
            (m_curChar.isNull()) ? QStringLiteral("EOL") : '\''_QL1
            + m_curChar + '\''_QL1;
        const QString &&charTxt =
            (chr.isNull()) ? QStringLiteral("EOL") : '\''_QL1 +
            chr + '\''_QL1;
        throwError((errMsg) ? errMsg : QT_TR_NOOP(
                       "Unexpected %1, expecting %2"),
                   { curCharTxt, charTxt }, m_pos, 1);
        return false;
    }
}

/*!
 * \brief Consumes the character \a chr and optionaly skips whitespaces.
 *
 * \sa expect()
 */
QString Parser::eat(QChar chr, const char *errMsg, EatOptions options) {
    const int start = m_pos;

    if (options.testFlag(SkipLeftWs)) {
        while (m_curChar.isSpace())
            advance();
    }

    expect(chr, errMsg);
    advance();

    if (options.testFlag(SkipRightWs)) {
        while (m_curChar.isSpace())
            advance();
    }

    return spanText(start);
}

/*!
 * \brief Returns the substring from the current character until it meets the character \a chr (exclusive).
 */
QStringView Parser::getUntil(QChar chr) {
    const auto view = peekUntil(chr);

    advance(view.length());
    return view;
}

/*!
 * \brief Returns the remaining substring from the current character.
 */
QStringView Parser::getRest() {
    const auto rest = m_text.mid(m_pos);

    advance(rest.length());
    return rest;
}

/*!
 * \brief Returns the substring from the current character with the given (regex) \a charset.
 */
QStringView Parser::getWithCharset(const QString &charset) {
    return getWithRegex(QLatin1String("[") + charset + QLatin1String("]+"));
}

QStringView Parser::getWithCharset(const QLatin1String &charset) {
    return getWithRegex(QLatin1String("[") + charset + QLatin1String("]+"));
}

/*!
 * \brief Returns the substring from the current character with the given regex \a pattern.
 */
QStringView Parser::getWithRegex(const QString &pattern) {
    return getWithRegex(QRegularExpression(pattern));
}

/*!
 * \brief Returns the substring from the current character with the given \a regex.
 */
QStringView Parser::getWithRegex(const QRegularExpression &regex) {
    const auto &&match = regex.match(m_text, m_pos,
                                     QRegularExpression::NormalMatch,
                                     QRegularExpression::AnchoredMatchOption);

    if (match.hasMatch()) {
        const int start = m_pos;
        advance(match.capturedLength());
// match.capturedView() return an invalid QStringView when a debugger is attached.
// return match.capturedView();
        return m_text.mid(start, match.capturedLength());
    } else {
        return QStringView();
    }
}

/*!
 * \brief Returns \a n next characters (including the current character) without
 * advancing the current position.
 */
QStringView Parser::peek(int n) const {
    return m_text.mid(m_pos, n);
}

/*!
 * \brief Returns \a n next characters (excluding the current character) without
 * advancing the current position.
 */
QStringView Parser::peekNext(int n) const {
    return m_text.mid(m_pos + 1, n);
}

/*!
 * \brief Returns the substring from the current character until it meets the
 * character \a chr (exclusive). It will not advances the current position.
 */
QStringView Parser::peekUntil(QChar chr) const {
    const int start = m_pos;
    const int index = m_text.indexOf(chr, start);

    return m_text.mid(start, (index > -1) ? index - start : m_text.size());
}

QStringView Parser::peekRest() const {
    return m_text.mid(m_pos);
}

/*!
 * \brief Skips and returns subsequent whitespaces.
 * If \a once is true, only one whitespace is skipped.
 */
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
    const static QVector<QChar> delimiters{ '"', '\'' };

    QChar curQuoteChar;

    if (delimiters.contains(m_curChar))
        curQuoteChar = m_curChar;
    eat(curQuoteChar);
    QString value;
    bool    backslash = false;
    while ((m_curChar != curQuoteChar) || backslash) {
        if (m_pos >= m_text.length())
            throwError(QT_TR_NOOP("Incomplete quoted string"));
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

                    case 'u': {
                        advance();
                        const auto &literal = peek(4);
                        bool        ok;
                        ushort      codepoint = literal.toUShort(&ok, 16);
                        if (ok) {
                            value += QChar(codepoint);
                            advance(3);
                        } else {
                            throwError(QT_TR_NOOP("Invalid Unicode code point"),
                                       {},
                                       pos() - 2,
                                       6);
                        }
                        break;
                    }

                    case 'r': {
                        value += '\r';
                        break;
                    }

                    case '/': {
                        value += '/';
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

QString Parser::spanText(QStringView textView) {
//    return spanText(textView.toString());

    const auto &&it = m_spans.constFind(textView);

    if (it != m_spans.cend()) {
        return *it;
    } else {
        const QString &&copy = textView.toString();
        return *m_spans.insert(copy, std::move(copy));
    }

//    const auto &&it = m_spans.find(textView);

//    if (it != m_spans.cend()) {
//        return it->second;
//    } else {
//        const QString &&copy = textView.toString();
//        return m_spans.emplace(copy, std::move(copy)).first->second;
//    }
}

QString Parser::spanText(const QString &text) {
//    return text;

//    return *m_spans.insert(text);

//    m_spans.insert(text);
//    return *m_spans.find(text);

    return *m_spans.insert(text, text);
//    return m_spans.try_emplace(text, text).first->second;
}

QString Parser::spanText(QString &&text) {
//    return text;

//    return *m_spans.insert(std::move(text));

//    m_spans.insert(std::move(text));
//    return *m_spans.find(text);

    return *m_spans.insert(text, std::move(text));
//    return m_spans.try_emplace(text, std::move(text)).first->second;
}

QString Parser::spanText(int start) {
    return spanText(m_text.mid(start, m_pos - start));
}

bool Parser::parse() {
    m_errors.clear();
    setPos(0);
    return parseImpl();
}

bool Parser::parse(const QString &text) {
    setText(text);
    return parse();
}

bool Parser::parse(QString &&text) {
    setText(std::move(text));
    return parse();
}

bool Parser::parse(QStringView text) {
    setText(text);
    return parse();
}
