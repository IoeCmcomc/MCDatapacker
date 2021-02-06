#include "parser.h"

#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMetaMethod>

QJsonObject Command::Parser::m_schema = {};

Command::Parser::ParsingError::ParsingError(const QString &what_arg)
    : runtime_error(what_arg.toStdString()) {
    message = what_arg;
}

Command::Parser::Parser(QObject *parent, const QString &input)
    : QObject(parent) {
    setText(input);

    /*printMethods(); */
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
    QFileInfo finfo = QFileInfo(filepath);

    if (!(finfo.exists() && finfo.isFile())) {
        qWarning() << "File not exists:" << finfo.filePath();
        return;
    }
    QFile inFile(finfo.filePath());
    inFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = inFile.readAll();
    inFile.close();

    QJsonParseError errorPtr{};
    QJsonDocument   doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        qWarning() << "Parse failed" << errorPtr.error;
        return;
    }
    QJsonObject root = doc.object();
    if (root.isEmpty()) {
        qWarning() << "Root is empty.";
        return;
    }

    setSchema(root);
}

/*!
 * \brief Returns a pointer to the parsing result as a \c RootNode.
 */
Command::RootNode *Command::Parser::parsingResult() {
    return &m_parsingResult;
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
        m_curChar = m_text[pos];
}

/*!
 * \brief Throws a \c Command::Parser::ParsingError with a formatted message.
 */
void Command::Parser::error(QStringView msg) {
    qWarning() << "Parsing error:" << msg;
    QString errorIndicatorText =
        QString("\"%1«%2»%3\" (%4 chars)").arg(
            m_text.left(m_pos - 1),
            m_curChar,
            m_text.mid(m_pos + 1),
            QString::number(m_text.length()));

    throw Command::Parser::ParsingError(tr(
                                            "Input: %1\nparser error at pos %2: %3").arg(
                                            errorIndicatorText,
                                            QString
                                            ::number(
                                                m_pos), msg));
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
    /*qDebug() << "Command::Parser::expect" << chr << acceptNull << m_curChar; */
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
            error(tr("Unexpected %1, expecting %2 or end of line").arg(
                      m_curChar, charTxt));
        else
            error(tr("Unexpected %1, expecting %2").arg(m_curChar,
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

/*!
 * \brief Returns the substring from the current character with the given regex. \a pattern.
 */
QString Command::Parser::getWithRegex(const QString &pattern) {
    QString ret;

    auto match = QRegularExpression(pattern)
                 .match(m_text, m_pos,
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

void Command::Parser::skipWs() {
    if (m_curChar.isSpace())
        advance();
}

/*!
 * \brief Returns the next literal string (word) without advancing the current pos.
 */
QString Command::Parser::peekLiteral() {
    QRegularExpression regex(R"([\S]+)");

    if (auto match =
            regex.match(m_text, m_pos, QRegularExpression::NormalMatch,
                        QRegularExpression::AnchoredMatchOption);
        match.hasMatch()) {
        return match.captured();
    }
    return "";
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
            error("Incomplete quoted string: " + value);
        if (backslash) {
            if (m_curChar == curQuoteChar)
                value += curQuoteChar;
            else if (m_curChar == '\\')
                value += '\\';
            else
                value += m_curChar;
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

QString Command::Parser::parserIdToMethodName(const QString &str) {
    if (str.contains(':')) {
        QStringList splited = str.split(':');
        QStringList words   = splited[1].split('_');
        QString     ret     = splited[0] + '_' + words[0];
        for (int i = 1; i < words.size(); ++i) {
            const QString &word = words[i];
            ret += word[0].toUpper() + word.midRef(1);
        }
        return ret;
    }
    return "";
}

Command::BoolNode *Command::Parser::brigadier_bool(QObject *parent,
                                                   [[maybe_unused]] const QVariantMap &)
{
    int start = m_pos;

    if (peek(4) == "true") {
        advance(4);
        return new Command::BoolNode(parent, start, true);
    } else if (peek(5) == "false") {
        advance(5);
        return new Command::BoolNode(parent, start, false);
    } else {
        return new Command::BoolNode(this); /* Returns an invaild node */
    }
}

Command::DoubleNode *Command::Parser::brigadier_double(QObject *parent,
                                                       const QVariantMap &props)
{
    int start = m_pos;

    QString lit   = getWithCharset("-0-9.eE");
    bool    ok    = false;
    double  value = lit.toDouble(&ok);

    if (!ok)
        error(tr("%1 is not a vaild double number").arg(lit));
    if (QVariant vari = props.value("min"); vari.isValid())
        checkMin(value, vari.toDouble());
    if (QVariant vari = props.value("max"); vari.isValid())
        checkMax(value, vari.toDouble());
    advance(lit.length());
    return new Command::DoubleNode(parent, start, lit.length(), value);
}

Command::FloatNode *Command::Parser::brigadier_float(QObject *parent,
                                                     const QVariantMap &props) {
    int     start = m_pos;
    QString raw   =
        getWithRegex(R"([+-]?(?:\d+\.\d+|\.\d+|\d+\.|\d+)(?:[eE]\d+)?)");
    bool  ok    = false;
    float value = raw.toFloat(&ok);

    if (!ok)
        error(tr("%1 is not a vaild float number").arg(raw));
    if (QVariant vari = props.value("min"); vari.isValid())
        checkMin(value, vari.toFloat());
    if (QVariant vari = props.value("max"); vari.isValid())
        checkMax(value, vari.toFloat());
    advance(raw.length());
    return new Command::FloatNode(parent, start, raw.length(), value);
}

Command::IntegerNode *Command::Parser::brigadier_integer(QObject *parent,
                                                         const QVariantMap &props)
{
    int     start = m_pos;
    QString raw   = getWithRegex(R"([+-]?\d+)");
    bool    ok    = false;
    int     value = raw.toFloat(&ok);

    if (!ok)
        error(tr("%1 is not a vaild integer number").arg(raw));
    if (QVariant vari = props.value("min"); vari.isValid())
        checkMin(value, vari.toInt());
    if (QVariant vari = props.value("max"); vari.isValid())
        checkMax(value, vari.toInt());
    advance(raw.length());
    return new Command::IntegerNode(parent, start, raw.length(), value);
}

Command::LiteralNode *Command::Parser::brigadier_literal(QObject *parent,
                                                         const QVariantMap &props)
{
    int start = m_pos;

    return new Command::LiteralNode(parent, start, getWithRegex(R"([\S]+)"));
}

Command::StringNode *Command::Parser::brigadier_string(QObject *parent,
                                                       const QVariantMap &props)
{
    auto *defaultRet = new Command::StringNode(parent, m_pos);

    if (!props.contains("type"))
        return defaultRet;

    QString type = props["type"].toString();
    if (type == "greedy") {
        auto *ret = new Command::StringNode(parent, m_pos, m_text.mid(m_pos));
        setPos(m_text.length());
        return ret;
    } else if (type == "phrase") {
        if (m_curChar == '"' || m_curChar == '\'')
            return new Command::StringNode(parent, m_pos, getQuotedString());
    } else if (type == "word") {
        return brigadier_literal(parent)->toStringNode(true);
    }
    return defaultRet;
}

/*!
 * \brief Parses the current text using the static schema. Returns the \c parsingResult or an invaild \c ParseNode if an error occured.
 */
Command::ParseNode *Command::Parser::parse() {
    qDebug() << "Command::Parser::parse" << m_text;
    if (m_schema.isEmpty()) {
        qWarning() << "The parser schema hasn't been initialized";
        new Command::ParseNode(this);
    }
    m_parsingResult.clear();
    m_parsingResult.setPos(0);
    try {
        if (parseResursively(this, m_schema)) {
            return &m_parsingResult;
        }
    } catch (const Command::Parser::ParsingError &err) {
        qDebug() << err.message;
        m_lastError = err;
    }
    return new Command::ParseNode(this);
}

Command::Parser::ParsingError Command::Parser::lastError() const {
    return m_lastError;
}

int Command::Parser::pos() const {
    return m_pos;
}

#define IF_TYPE_BRANCH(Type)         if (type == qMetaTypeId<Type*>()) \
    root->prepend(qvariant_cast<Type*>(vari));
#define ELSE_IF_TYPE_BRANCH(Type)    else IF_TYPE_BRANCH(Type)

/*!
 * \brief Casts the data in \a vari to its original type and prepend it to the RootNode \a root.
 */
bool Command::Parser::castThenPrependTo(QVariant &vari,
                                        Command::RootNode *root) {
    /*qDebug() << "canConvert:" << vari.canConvert<Command::ParseNode*>(); */
    if (!vari.canConvert<Command::ParseNode*>())
        return false;

    int type = vari.userType();

    IF_TYPE_BRANCH(Command::RootNode)
    ELSE_IF_TYPE_BRANCH(Command::BoolNode)
    ELSE_IF_TYPE_BRANCH(Command::DoubleNode)
    ELSE_IF_TYPE_BRANCH(Command::FloatNode)
    ELSE_IF_TYPE_BRANCH(Command::IntegerNode)
    ELSE_IF_TYPE_BRANCH(Command::LiteralNode)
    ELSE_IF_TYPE_BRANCH(Command::ParseNode)
    ELSE_IF_TYPE_BRANCH(Command::StringNode)
    ELSE_IF_TYPE_BRANCH(Command::ArgumentNode)
    else return false;

    return true;
}


bool Command::Parser::parseResursively(QObject *parentObj,
                                       QJsonObject curSchemaNode,
                                       int depth) {
/*
      qDebug() << "Command::Parser::parseResursively" << parentObj <<
          curSchemaNode << depth;
 */

    Command::ParseNode                    *ret;
    QVector<Command::Parser::ParsingError> errors;
    QVector<int>                           argLengths;

    if (depth > 0) {
        if (depth > 100)
            qWarning() << "The parsing stack depth is too large:" << depth;
/*
          qDebug() << "has children:" << curSchemaNode.contains("children");
          qDebug() << "has executable:" << curSchemaNode.contains("executable");
          qDebug() << "has redirect:" << curSchemaNode.contains("redirect");
 */
        if (!curSchemaNode.contains("children")) {
            if (!curSchemaNode.contains("executable")
                || (curSchemaNode.contains("executable")
                    && curSchemaNode.contains("redirect"))) {
                if (this->m_curChar.isNull()) {
                    QJsonArray redirect;
                    if (curSchemaNode.contains("redirect"))
                        redirect = curSchemaNode["redirect"].toArray();
                    curSchemaNode = m_schema;
                    for (const auto &nodeNameRef: redirect) {
                        curSchemaNode =
                            curSchemaNode["children"]
                            .toObject()[nodeNameRef.toString()].toObject();
                    }
                    ;
                } else {
                    this->error(tr("Incompleted command"));
                }
            } else {
                this->expect(QChar());
                return true;
            }
        } else {
            if (curSchemaNode.contains("executable")) {
                if (this->m_curChar.isNull())
                    return true;
            }
        }
        this->eat(' ');
    } else {
        curSchemaNode = m_schema;
    }
    int     startPos = this->m_pos;
    QString literal  = this->peekLiteral();
    qDebug() << "literal:" << literal;
    bool found    = false;
    auto children = curSchemaNode["children"].toObject();
    for (auto it = children.constBegin(); it != children.constEnd(); it++) {
        curSchemaNode = it.value().toObject();
        if (curSchemaNode["type"] == "literal") {
            if (literal == it.key()) {
                found = true;
                ret   = this->brigadier_literal(parentObj);
                this->parseResursively(&m_parsingResult,
                                       curSchemaNode,
                                       depth + 1);
                this->m_parsingResult.prepend(ret);
                break;
            }
        } else if (curSchemaNode["type"].toString() == "argument") {
            QString parserId    = curSchemaNode["parser"].toString();
            auto    methodName  = parserIdToMethodName(parserId).toLatin1();
            int     methodIndex = metaObject()->indexOfMethod(
                methodName + "(QObject*,QVariantMap)");
            if (methodIndex != -1) {
                auto method = metaObject()->method(methodIndex);
                auto props  =
                    curSchemaNode["properties"].toObject().toVariantMap();
                int      returnType = method.returnType();
                auto     typeName   = method.typeName();
                QVariant vari(returnType, nullptr);
                void    *data = vari.data();
                qDebug() << returnType << vari << data;
                try {
                    bool invoked =
                        method.invoke(this,
                                      QGenericReturnArgument(typeName, data),
                                      Q_ARG(QObject*, &m_parsingResult),
                                      Q_ARG(QVariantMap, props));
                    qDebug() << invoked << vari << data;
                    if (invoked && data) {
                        found = true;
                        ret   = vari.value<Command::ParseNode*>();
                        qDebug() << ret << found;
                        bool ok = this->parseResursively(&m_parsingResult,
                                                         curSchemaNode,
                                                         depth + 1);
                        /*qDebug() << "ok:" << ok; */
                        if (!ok)
                            qWarning() << "parseResursively returns false";
                        castThenPrependTo(vari, &m_parsingResult);
                        break;
                    } else {
                        qWarning() << "Method not invoked:" << methodName;
                    }
                } catch (const Command::Parser::ParsingError &err) {
                    errors << err;
                    int argLength = this->m_pos - startPos;
                    if (argLength > 0) {
                        this->recede(argLength);
                    }
                    argLengths << argLength;
                }
            } else {
                this->error("Unknown parser " + parserId);
            }
        }
    }
    if (ret && ret->pos() == -2) { /* If succeed */
    } else if (errors.size() == curSchemaNode.size()) {
        if (errors.size() == 1) {
            throw errors[0];
        } else {
            bool sameAll =
                std::all_of(argLengths.cbegin(), argLengths.cend(),
                            [argLengths](int v) {
                return v == argLengths[0];
            });
            if (sameAll) {
                throw new Command::Parser::ParsingError("Incorrect argument");
            } else {
                throw errors[*std::max_element(argLengths.cbegin(),
                                               argLengths.cend())];
            }
        }
    }
    if (!found) {
        if (curSchemaNode["type"] != "root") {
            this->error("Unknown literal '" + literal + "'");
        } else {
            this->error("Unknown command '" + literal + "'");
        }
    }
    return true;
}
