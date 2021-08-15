#include "parser.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMetaMethod>
#include <QElapsedTimer>

Command::Parser::Error::Error(const QString &whatArg, int pos, int length,
                              const QVariantList &args)
    : std::runtime_error(whatArg.toStdString()) {
    this->pos    = pos;
    this->length = length;
    this->args   = args;
}

QString Command::Parser::Error::toLocalizedMessage() const {
    QString &&errMsg =
        QCoreApplication::translate("Command::Parser::Error", what());

    if (errMsg == what())
        errMsg = QCoreApplication::translate("Command::Parser", what());

    qDebug() << what() << errMsg;

    for (int i = 0; i < args.size(); ++i) {
        errMsg = errMsg.arg(args.at(i).toString());
    }

    const QString &&ret =
        QCoreApplication::translate("Command::Parser",
                                    "Syntax error at position %1: %2")
        .arg(pos).arg(errMsg);
    return std::move(ret);
}

QJsonObject Command::Parser::m_schema   = {};
bool        Command::Parser::m_testMode = false;

Command::Parser::Parser(QObject *parent, const QString &input)
    : QObject(parent) {
    setText(input);

/*    printMethods(); */
}

void Command::Parser::printMethods() {
    qDebug() << metaObject()->methodCount();
    for (int i = 0; i < metaObject()->methodCount(); ++i) {
        QMetaMethod info = metaObject()->method(i);
        qDebug() << info.isValid() << info.name() << info.methodType() <<
            info.methodSignature() << info.parameterTypes() <<
            info.typeName() << info.parameterNames() << info.returnType();
    }
}

const Command::ParseNodeCache &Command::Parser::cache() const {
    return m_cache;
}

QChar Command::Parser::curChar() const {
    return m_curChar;
}

QJsonObject Command::Parser::getSchema() {
    return m_schema;
}

void Command::Parser::setSchema(const QJsonObject &schema) {
    m_schema = schema;
}

/*!
 * \brief Opens a JSON file and loads it into the static schema.
 */
void Command::Parser::setSchema(const QString &filepath) {
    QFileInfo finfo(filepath);

    if (!(finfo.exists() && finfo.isFile())) {
        qWarning() << "File not exists:" << finfo.filePath();
        return;
    }
    QFile inFile(finfo.filePath());
    inFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray &&data = inFile.readAll();
    inFile.close();

    QJsonParseError errorPtr{};
    QJsonDocument &&doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        qWarning() << "Parsing failed" << errorPtr.error;
        return;
    }
    QJsonObject &&root = doc.object();
    if (root.isEmpty()) {
        qWarning() << "Root is empty.";
        return;
    }
    setSchema(root);
}

/*!
 * \brief Returns a shared pointer to the parsing result as a \c RootNode.
 */
QSharedPointer<Command::RootNode> Command::Parser::parsingResult() {
    return m_parsingResult;
}

/*!
 * \brief Returns the text which is parsed.
 */
QString Command::Parser::text() const {
    return m_text;
}

/*!
 * \brief Sets the text which is parsed and resets the current position.
 */
void Command::Parser::setText(const QString &text) {
    m_text = text;
    setPos(0);
}

/*!
 * \brief Sets the current position to parse to \a pos
 * and updates the current character.
 */
void Command::Parser::setPos(int pos) {
    m_pos = pos;
    if (pos < 0)
        qWarning("The current position will be less than 0");
    if (pos >= m_text.length())
        m_curChar = QChar();
    else
        m_curChar = m_text.at(pos);
}

/*!
 * \brief Gets the last parse error of the parser.
 */
Command::Parser::Error Command::Parser::lastError() const {
    return m_lastError;
}

/*!
 * \brief Gets the current position of the parser.
 */
int Command::Parser::pos() const {
    return m_pos;
}

/*!
 * \brief Throws a \c Command::Parser::ParsingError with a formatted message.
 */
void Command::Parser::error(const QString &msg, const QVariantList &args) {
    error(msg, args, m_pos);
}

void Command::Parser::error(const QString &msg, const QVariantList &args,
                            int pos, int length) {
    qWarning() << "Command::Parser::error" << msg << pos << length;
/*
      QString errorIndicatorText = QString("\"%1«%2»%3\" (%4 chars)").arg(
          m_text.mid(pos - 10, 10),
          m_text.mid(pos, length),
          m_text.mid(pos + length, 10),
          QString::number(m_text.length()));
 */

    throw Command::Parser::Error(msg, pos, length, args);
}

/*!
 * \brief Advances \a n characters, then updates the current character.
 */
void Command::Parser::advance(int n) {
    if (n < 1)
        return;

    setPos(m_pos + n);
}

/*!
 * \brief recedes \a n characters, then updates the current character.
 */
void Command::Parser::recede(int n) {
    if (n < 1)
        return;

    setPos(m_pos - n);
}

/*!
 * \brief throws a \c error if the char \a chr isn't equal to the current character.
 */
bool Command::Parser::expect(const QChar &chr, bool acceptNull) {
/*    qDebug() << "Command::Parser::expect" << chr << acceptNull << m_curChar; */
    bool cond = m_curChar == chr;

    if (!cond && acceptNull)
        cond = m_curChar.isNull();
    if (cond) {
        return true;
    } else {
        const QString &&curCharTxt =
            (m_curChar.isNull()) ? QLatin1String("EOL") : '\'' + m_curChar +
            '\'';
        const QString &&charTxt =
            (chr.isNull()) ? QLatin1String("EOL") : '\'' + chr + '\'';
        error(QT_TR_NOOP("Unexpected %1, expecting %2"),
              { curCharTxt, charTxt }, m_pos, 1);
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
/*    QString ret; */

    const int pos     = m_pos;
    const int textLen = m_text.length();
    int       len     = -1;

    do {
        ++len;
        if ((pos + len) >= textLen)
            break;
    } while (m_text.at(pos + len) != chr);

    advance(len);
    return m_text.mid(pos, len);
}

/*!
 * \brief Returns the remaining substring from the current character.
 */
QString Command::Parser::getRest() {
    const QString &&rest = m_text.mid(m_pos);

    advance(rest.length());
    return rest;
}

/*!
 * \brief Returns the substring from the current character with the given (regex) \a charset.
 */
QString Command::Parser::getWithCharset(const QString &charset) {
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
QString Command::Parser::getWithRegex(const QString &pattern) {
    return getWithRegex(QRegularExpression(pattern));
}

/*!
 * \brief Returns the substring from the current character with the given \a regex.
 */
QString Command::Parser::getWithRegex(const QRegularExpression &regex) {
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

QString Command::Parser::peek(int n) {
    return m_text.mid(m_pos, n);
}

void Command::Parser::skipWs(bool once) {
    while (m_curChar.isSpace()) {
        advance();
        if (once)
            break;
    }
}

/*!
 * \brief Returns the next literal string (word) without advancing the current pos.
 */
QString Command::Parser::peekLiteral() {
    const static QRegularExpression regex(R"([\S]+)");

    if (auto match =
            regex.match(m_text, m_pos, QRegularExpression::NormalMatch,
                        QRegularExpression::AnchoredMatchOption);
        match.hasMatch()) {
        return match.captured();
    }
    return QString();
}

/*!
 * \brief Returns the next quoted string.
 */
QString Command::Parser::getQuotedString() {
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

/*!
 * \brief Converts the specified parser ID into a method name to use with Qt's meta method system.
 */
QString Command::Parser::parserIdToMethodName(const QString &str) {
    if (str.contains(':')) {
        QStringList &&splited = str.split(':');
        QStringList &&words   = splited[1].split('_');
        for (int i = 1; i < words.size(); ++i) {
            words[i].replace(0, 1, words[i][0].toUpper());
        }
        return splited[0] + '_' + words.join(QString());
    }
    return QString();
}

QSharedPointer<Command::BoolNode> Command::Parser::brigadier_bool() {
    int start = m_pos;

    if (peek(4) == QLatin1String("true")) {
        advance(4);
        return QSharedPointer<Command::BoolNode>::create(start, true);
    } else if (peek(5) == QLatin1String("false")) {
        advance(5);
        return QSharedPointer<Command::BoolNode>::create(start, false);
    } else {
        return QSharedPointer<Command::BoolNode>::create(); /* Returns an invaild node */
    }
}

QSharedPointer<Command::DoubleNode> Command::Parser::brigadier_double(
    const QVariantMap &props) {
    int start = m_pos;

    QString &&raw   = getWithRegex(m_decimalNumRegex);
    bool      ok    = false;
    double    value = raw.toDouble(&ok);

    if (!ok)
        error(QT_TR_NOOP("%1 is not a vaild double number"), { raw });
    if (const QVariant &vari = props.value(QStringLiteral(
                                               "min")); vari.isValid()) {
        checkMin(value, vari.toDouble());
    }
    if (const QVariant &vari = props.value(QStringLiteral(
                                               "max")); vari.isValid()) {
        checkMax(value, vari.toDouble());
    }
    return QSharedPointer<Command::DoubleNode>::create(start, raw.length(),
                                                       value);
}

QSharedPointer<Command::FloatNode> Command::Parser::brigadier_float(
    const QVariantMap &props) {
    int       start = m_pos;
    QString &&raw   = getWithRegex(m_decimalNumRegex);
    bool      ok    = false;
    float     value = raw.toFloat(&ok);

    if (!ok)
        error(QT_TR_NOOP("%1 is not a vaild float number"), { raw });
    if (const QVariant &vari = props.value(QStringLiteral(
                                               "min")); vari.isValid()) {
        checkMin(value, vari.toFloat());
    }
    if (const QVariant &vari = props.value(QStringLiteral(
                                               "max")); vari.isValid()) {
        checkMax(value, vari.toFloat());
    }
    return QSharedPointer<Command::FloatNode>::create(start, raw.length(),
                                                      value);
}

QSharedPointer<Command::IntegerNode> Command::Parser::brigadier_integer(
    const QVariantMap &props) {
    int       start = m_pos;
    QString &&raw   = getWithRegex(R"([+-]?\d+)");
    bool      ok    = false;
    int       value = raw.toFloat(&ok);

    if (!ok)
        error(QT_TR_NOOP("%1 is not a vaild integer number"), { raw });
    if (const QVariant &vari = props.value(QStringLiteral(
                                               "min")); vari.isValid()) {
        checkMin(value, vari.toInt());
    }
    if (const QVariant &vari = props.value(QStringLiteral(
                                               "max")); vari.isValid()) {
        checkMax(value, vari.toInt());
    }
    return QSharedPointer<Command::IntegerNode>::create(
        start, raw.length(), value);
}

QSharedPointer<Command::LiteralNode> Command::Parser::brigadier_literal() {
    int             start   = m_pos;
    const QString &&literal = getWithRegex(m_literalStrRegex);
    const int       typeId  = qMetaTypeId<QSharedPointer<LiteralNode> >();
    CacheKey        key{ typeId, literal, start };

    if (m_cache.contains(key)) {
        return qSharedPointerCast<LiteralNode>(m_cache[key]);
    } else if (key.pos = -1; m_cache.contains(key)) {
        auto &&ret = QSharedPointer<LiteralNode>::create(*qSharedPointerCast<LiteralNode>(
                                                             m_cache[key]));
        ret->setPos(start);
        return std::move(ret);
    } else {
        auto &&ret =
            QSharedPointer<Command::LiteralNode>::create(start, literal);
        m_cache.emplace(typeId, literal, ret);
        return std::move(ret);
    }
}

QSharedPointer<Command::StringNode> Command::Parser::brigadier_string(
    const QVariantMap &props) {
    auto defaultRet = QSharedPointer<Command::StringNode>::create(m_pos);

    if (!props.contains(QLatin1String("type")))
        return defaultRet;

    QString &&type = props[QLatin1String("type")].toString();
    if (type == QLatin1String("greedy")) {
        return QSharedPointer<Command::StringNode>::create(
            m_pos, getRest());;
    } else if (type == QLatin1String("phrase")) {
        if (m_curChar == '"' || m_curChar == '\'')
            return QSharedPointer<Command::StringNode>::create(
                m_pos, getQuotedString(), true);
    } else if (type == QLatin1String("word")) {
        const auto &literal = brigadier_literal();
        return Command::StringNode::fromLiteralNode(literal.get());
    }
    return defaultRet;
}

/*!
 * \brief Parses the current text using the static schema. Returns the \c parsingResult or an invaild \c ParseNode if an error occured.
 */
QSharedPointer<Command::ParseNode> Command::Parser::parse() {
    m_parsingResult = QSharedPointer<RootNode>::create(pos());
    QElapsedTimer timer;
    timer.start();

    if (m_schema.isEmpty()) {
        qWarning() << "The parser schema hasn't been initialized yet.";
    } else if (text().trimmed().isEmpty() || text().at(0) == '#') {
        m_parsingResult->setPos(0);
        return m_parsingResult;
    }
    setPos(0);
    const int typeId = qMetaTypeId<QSharedPointer<RootNode> >();
    CacheKey  key{ typeId, m_text, (!Parser::m_testMode) ? -1 : 1 };

    if (m_cache.contains(key)) {
        m_parsingResult = qSharedPointerCast<RootNode>(m_cache[key]);
/*
          qDebug() << "Got cached" << m_parsingResult << "in ms:" <<
              timer.elapsed();
 */
        setPos(m_text.length());
    } else {
        try {
            if (parseResursively(m_schema)) {
                m_parsingResult->setPos(0);
                m_parsingResult->setLength(pos() - 1);
/*
                  qDebug() << "Parsed line" << m_parsingResult << "in ms:" <<
                      timer.elapsed();
 */

                m_cache.emplace(typeId, m_text, m_parsingResult);
            }
        } catch (const Command::Parser::Error &err) {
            qDebug() << "Command::Parser::parse" << err.toLocalizedMessage();
            m_lastError = err;
            m_parsingResult->setPos(-1); /* Make the result invaild */
        }
    }
    return m_parsingResult;
}

/*!
 * \brief Try to cast a void pointer in \c vari to a QSharedPointer of ParseNode's subclass.
 */
QSharedPointer<Command::ParseNode> Command::Parser::
QVariantToParseNodeSharedPointer(
    const QVariant &vari) {
    unsigned short int typeId = vari.userType();

    QVARIANT_CAST_SHARED_POINTER_IF_BRANCH(ParseNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(RootNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ArgumentNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(BoolNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(DoubleNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(FloatNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(IntegerNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(StringNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(LiteralNode)
    return nullptr;
}

bool Command::Parser::processCurSchemaNode(int depth,
                                           QJsonObject &curSchemaNode) {
    if (depth > 0) {
        if (depth > 100)
            qWarning() << "The parsing stack depth is too large:" << depth;
/*
          qDebug() << "has children:" << curSchemaNode.contains("children");
          qDebug() << "has executable:" << curSchemaNode.contains("executable");
          qDebug() << "has redirect:" << curSchemaNode.contains("redirect");
 */
        if (!curSchemaNode.contains(QLatin1String("children"))) {
            if (!curSchemaNode.contains(QLatin1String("executable"))
                || (curSchemaNode.contains(QLatin1String("executable"))
                    && curSchemaNode.contains(QLatin1String("redirect")))) {
                if (!m_curChar.isNull()) {
                    QJsonArray redirect;
                    if (curSchemaNode.contains(QLatin1String("redirect")))
                        redirect =
                            curSchemaNode[QLatin1String("redirect")].toArray();
                    curSchemaNode = m_schema;
                    for (const auto &nodeNameRef: qAsConst(redirect)) {
                        curSchemaNode =
                            curSchemaNode[QLatin1String("children")]
                            .toObject()[nodeNameRef.toString()].toObject();
                    }
                } else {
                    error(QT_TR_NOOP("Incompleted command"));
                }
            } else {
                expect(QChar());
                return true;
            }
        } else {
            if (curSchemaNode.contains(QLatin1String("executable"))) {
                if (m_curChar.isNull())
                    return true;
            }
        }
        if (curChar().isNull())
            error(QT_TR_NOOP("Incompleted command"));
        eat(' ');
    }
    return false; /* Should continue parsing */
}

bool Command::Parser::parseResursively(QJsonObject curSchemaNode,
                                       int depth) {
/*
      qDebug() << "Command::Parser::parseResursively"
   << curSchemaNode << depth;
 */

    QSharedPointer<Command::ParseNode> ret = nullptr;
    QVector<Command::Parser::Error>    errors;
    QVector<int>                       argLengths;

    if (processCurSchemaNode(depth, curSchemaNode))
        return true;

    bool isRoot = curSchemaNode[QLatin1String("type")] ==
                  QStringLiteral("root");

    int       startPos = m_pos;
    bool      success  = false;
    QString &&literal  = peekLiteral();
    /*qDebug() << "literal:" << literal; */
    bool         found    = false;
    const auto &&children = curSchemaNode[QLatin1String("children")].toObject();
    for (auto it = children.constBegin(); it != children.constEnd(); ++it) {
        curSchemaNode = it.value().toObject();
        if (curSchemaNode[QLatin1String("type")] == QStringLiteral("literal")) {
            if (literal == it.key()) {
                found   = true;
                ret     = brigadier_literal();
                success = parseResursively(curSchemaNode, depth + 1);
                m_parsingResult->prepend(ret);
                break;
            }
        } else if (curSchemaNode[QLatin1String("type")] ==
                   QStringLiteral("argument")) {
            QString &&parserId = curSchemaNode["parser"].toString();
            int       methodIndex;
            if (curSchemaNode.contains(QLatin1String("properties"))) {
                methodIndex = metaObject()->indexOfMethod(
                    parserIdToMethodName(parserId).toLatin1() +
                    "(QVariantMap)");
            } else {
                methodIndex = metaObject()->indexOfMethod(
                    parserIdToMethodName(parserId).toLatin1() + "()");
            }
            if (methodIndex != -1) {
                /*qDebug() << "Argument parser ID:" << parserId; */
                auto        &&method     = metaObject()->method(methodIndex);
                int           returnType = method.returnType();
                QElapsedTimer timer;
                timer.start();
                CacheKey key{ returnType, literal, startPos };

                try {
                    if (m_cache.contains(key)) {
                        found = true;
                        ret   = m_cache[key];
/*
                          qDebug() << "Cached: " << ret << found << ret->pos() <<
                              ret->length() << "elapsed time:" << timer.elapsed();
 */
                    } else if (key.pos = -1; m_cache.contains(key)) { /* Not taking position into account */
                        /* Make a copy of the cached object */
                        ret = QSharedPointer<ParseNode>::create(*m_cache[key]);
                        /* Change its pos to the start pos */
                        ret->setPos(startPos);

/*
                          qDebug() << "Detached from cache: " << ret << found <<
                              ret->pos() <<
                              ret->length() << "elapsed time:" << timer.elapsed();
 */
                    }
                    if (found) {
                        Q_ASSERT(ret != nullptr);
                        advance(literal.length());
                        if ((literal.length() == ret->length()) &&
                            ret->isVaild())
                            m_cache.emplace(returnType, literal, startPos, ret);
                        success = parseResursively(curSchemaNode, depth + 1);
                        m_parsingResult->prepend(ret);
                        break;
                    }

                    auto props =
                        curSchemaNode[QLatin1String("properties")].toObject().
                        toVariantMap();

                    auto     typeName = method.typeName();
                    QVariant returnVari(returnType, nullptr);

                    QGenericReturnArgument
                        returnArgument(typeName,
                                       const_cast<void*>(returnVari.constData()));
                    bool invoked = method.invoke(this, returnArgument,
                                                 Q_ARG(QVariantMap, props));
                    /*qDebug() << invoked << returnVari; */
                    if (invoked) {
                        found = true;
                        ret   = QVariantToParseNodeSharedPointer(returnVari);
/*
                          qDebug() << ret << found << "elapsed time:" <<
                              timer.elapsed();
 */
                        Q_ASSERT(ret != nullptr);
                        if ((literal.length() == ret->length()) &&
                            ret->isVaild())
                            m_cache.emplace(returnType, literal, startPos, ret);
                        success = parseResursively(curSchemaNode, depth + 1);
                        m_parsingResult->prepend(ret);
                        break;
                    } else {
                        qWarning() << QLatin1String(
                            "Method not invoked: %1 (%2)").arg(parserIdToMethodName(
                                                                   parserId),
                                                               parserId);
                    }
                } catch (const Command::Parser::Error &err) {
                    qWarning() << "Argument error:" << err.toLocalizedMessage();
                    errors << err;
                    int argLength = m_pos - startPos + 1;
                    setPos(startPos);
                    found = false;
                    argLengths << argLength;
                }
            } else {
                error(QT_TR_NOOP("Unknown argument parser: '%1'"),
                      { parserId });
            }
        }
    }
    /*qDebug() << "After loop" << ret->isVaild() << success << errors.length() <<
       curSchemaNode.size(); */
    if (ret && ret->isVaild() && success) { /* If succeed */
    } else if (errors.size() == 1) {
        throw errors[0];
    } else if (errors.size() == curSchemaNode.size()) {
        bool sameAll =
            std::all_of(argLengths.cbegin(), argLengths.cend(),
                        [argLengths](int v) {
            return v == argLengths[0];
        });
        if (sameAll) {
            throw Command::Parser::Error(QT_TR_NOOP("Incorrect argument"));
        } else {
            throw errors[*std::max_element(argLengths.cbegin(),
                                           argLengths.cend())];
        }
    }
    if (!found) {
        if (!isRoot) {
            error(QT_TR_NOOP("Unknown subcommand '%1'"), { literal });
        } else {
            error(QT_TR_NOOP("Unknown command '%1'"), { literal });
        }
    }
    return true;
}
