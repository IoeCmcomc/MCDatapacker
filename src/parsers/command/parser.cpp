#include "parser.h"

#include "schema/schemaargumentnode.h"
#include "schema/schemaliteralnode.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMetaMethod>
#include <QElapsedTimer>

/* These classes are for benchmarking deserialization methods.
 * For now, deserializing from JSON using QJsonDocument (faster than from MessagePack).

 #include <QStack>

 #include <stack>

   using json = nlohmann::json;

   template <typename A, typename O>
   struct StackElement
   {
    enum class Type {
        Array,
        Object,
    };

    A array;
    O object;
    Type type = StackElement::Type::Array;
    QString lastKey;

    StackElement() {}

    StackElement(const A &arr) {
        type = StackElement::Type::Array;
        array = arr;
    }

    StackElement(const O &obj) {
        type = StackElement::Type::Object;
        object = obj;
    }

    StackElement(const StackElement &other) {
        type = other.type;
        if (type == StackElement::Type::Object) {
            object = other.object;
            lastKey = other.lastKey;
        } else {
            array = other.array;
        }
    };

    StackElement(StackElement &&other) {
        type = other.type;
        if (type == StackElement::Type::Object) {
            object = std::move(other.object);
            lastKey = std::move(other.lastKey);
        } else {
            array = std::move(other.array);
        }
    };
   };

   template <typename E, typename A, typename O>
   class SaxConsumer : public json::json_sax_t
   {
   public:
    O root;

    using Element = StackElement<A, O>;
    std::stack<Element> stack;

    bool null() override
    {
        auto &top = stack.top();
        if (top.type == Element::Type::Object) {
            top.object.insert(top.lastKey, E());
        } else {
            top.array << E();
        }
        return true;
    }

    bool boolean(bool val) override
    {
        auto &top = stack.top();
        if (top.type == Element::Type::Object) {
            top.object.insert(top.lastKey, val);
        } else {
            top.array << val;
        }
        return true;
    }

    bool number_integer(number_integer_t val) override
    {
        auto &top = stack.top();
        if (top.type == Element::Type::Object) {
            top.object.insert(top.lastKey, val);
        } else {
            top.array << val;
        }
        return true;
    }

    bool number_unsigned(number_unsigned_t val) override
    {
        if (stack.size() == 0) {
            qDebug() << root;
        }
        auto &top = stack.top();
        if (top.type == Element::Type::Object) {
            top.object.insert(top.lastKey, QVariant::fromValue(val).toJsonValue());
        } else {
            top.array << QVariant::fromValue(val).toJsonValue();
        }
        return true;
    }

    bool number_float(number_float_t val, const string_t& s) override
    {
        auto &top = stack.top();
        if (top.type == Element::Type::Object) {
            top.object.insert(top.lastKey, val);
        } else {
            top.array << val;
        }
        return true;
    }

    bool string(string_t& val) override
    {
        auto &top = stack.top();
        if (top.type == Element::Type::Object) {
            top.object.insert(top.lastKey, val.c_str());
        } else {
            top.array << QString::fromStdString(val);
        }
        return true;
    }

    bool start_object(std::size_t elements) override
    {
        stack.push(O());
        return true;
    }

    bool end_object() override
    {
        const auto element = std::move(stack.top());
        stack.pop();
        if (stack.size() == 0) {
            root = std::move(element.object);
        } else {
            auto &top = stack.top();
            if (top.type == Element::Type::Object) {
                top.object.insert(top.lastKey, element.object);
            } else {
                top.array << element.object;
            }
        }
        return true;
    }

    bool start_array(std::size_t elements) override
    {
        stack.push(A());
        return true;
    }

    bool end_array() override
    {
        const auto element = std::move(stack.top());
        stack.pop();
        auto &top = stack.top();
        if (top.type == Element::Type::Object) {
            top.object.insert(top.lastKey, element.array);
        } else {
            top.array << element.array;
        }
        return true;
    }

    bool key(string_t& val) override
    {
        auto &top = stack.top();
        top.lastKey = val.c_str();
        return true;
    }

    bool binary(json::binary_t& val) override
    {
        return false;
    }

    bool parse_error(std::size_t position, const std::string& last_token, const json::exception& ex) override
    {
        qWarning() << "parse_error(position=" << position << ", last_token=" << last_token.c_str() << ",\n            ex=" << ex.what() << ")";
        return false;
    }
   };

   Q_DECLARE_METATYPE(QVector<QVariant>)

   QVariant from_json(const json &v) {
    using Type = json::value_t;
    switch (v.type()) {
    case Type::object: {
        auto &&map = v.get<std::map<std::string, json>>();
        QVariantMap qmap;
        for (const auto &[k, v]: map) {
            qmap.insert(k.c_str(), from_json(v));
        }
        return qmap;
        break;
    }
    case Type::array: {
        auto &&vec = v.get<std::vector<json>>();
        QVector<QVariant> list;
        for (const auto &v: vec) {
            list << from_json(v);
        }
        QVariant vari;
        vari.setValue(list);
        return vari;
        break;
    }
    case Type::boolean: {
        return v.get<bool>();
        break;
    }
    case Type::string : {
        return v.get<std::string>().c_str();
        break;
    }
    case Type::number_integer: {
        return v.get<int>();
        break;
    }
    case Type::number_float: {
        return v.get<double>();
        break;
    }
    case Type::number_unsigned: {
        return v.get<unsigned int>();
        break;
    }
    case Type::null: {
        return QVariant();
        break;
    }
    default:
        return QVariant();
        break;
    }
   }

   class QtSaxConsumer: public SaxConsumer<QVariant, QVariantList, QVariantMap> {
   public:
    bool number_unsigned(number_unsigned_t val) override
    {
        auto &top = stack.top();
        if (top.type == Element::Type::Object) {
            top.object.insert(top.lastKey, val);
        } else {
            top.array << val;
        }
        return true;
    }
   };

 */

namespace Command {
    Parser::Error::Error(const QString &whatArg, int pos, int length,
                         const QVariantList &args)
        : std::runtime_error(whatArg.toStdString()) {
        this->pos    = pos;
        this->length = length;
        this->args   = args;
    }

    QString Parser::Error::toLocalizedMessage() const {
        QString &&errMsg =
            QCoreApplication::translate("Command::Parser::Error", what());

        if (errMsg == what())
            errMsg = QCoreApplication::translate("Command::Parser", what());

        for (int i = 0; i < args.size(); ++i) {
            errMsg = errMsg.arg(args.at(i).toString());
        }

        const QString &&ret =
            QCoreApplication::translate("Command::Parser",
                                        "Syntax error at position %1: %2")
            .arg(pos).arg(errMsg);
        return std::move(ret);
    }

    QJsonObject Parser::m_schema   = {};
    bool        Parser::m_testMode = false;

    Parser::Parser(QObject *parent, const QString &input)
        : QObject(parent) {
        setText(input);

/*    printMethods(); */
    }

    void Parser::printMethods() {
        qDebug() << metaObject()->methodCount();
        for (int i = 0; i < metaObject()->methodCount(); ++i) {
            QMetaMethod info = metaObject()->method(i);
            qDebug() << info.isValid() << info.name() << info.methodType() <<
                info.methodSignature() << info.parameterTypes() <<
                info.typeName() << info.parameterNames() << info.returnType();
        }
    }

    QString Parser::spanText(const QStringRef &text) {
        //qDebug() << "spanText(const QStringRef &text)" << text << m_pos <<
        //    m_curChar;
        auto it = std::find(m_spans.begin(), m_spans.end(), text);

        if (it != m_spans.end()) {
            return *it;
        } else {
            const QString &&newText = text.toString();
            m_spans.insert(newText);
            return newText;
        }
    }

    QString Parser::spanText(const QString &text) {
        //qDebug() << "spanText(const QString &text)" << text << m_pos <<
        //    m_curChar;
        auto it = std::find(m_spans.begin(), m_spans.end(), text);

        if (it != m_spans.end()) {
            return *it;
        } else {
            m_spans.insert(text);
            return text;
        }
    }

    QString Parser::spanText(int start) {
        return spanText(m_text.midRef(start, m_pos - start));
    }

    const ParseNodeCache &Parser::cache() const {
        return m_cache;
    }

    QChar Parser::curChar() const {
        return m_curChar;
    }

    QJsonObject Parser::getSchema() {
        return m_schema;
    }

    void Parser::setSchema(const QJsonObject &schema) {
        m_schema = schema;
    }

/*!
 * \brief Opens a JSON file and loads it into the static schema.
 */
    void Parser::setSchema(const QString &filepath) {
        QFileInfo finfo(filepath);

        if (!(finfo.exists() && finfo.isFile())) {
            qWarning() << "File not exists:" << finfo.filePath();
            return;
        }

        QFile inFile(finfo.filePath());
        inFile.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray &&data = inFile.readAll();
        inFile.close();

/*

    using QtJsonSaxConsumer = SaxConsumer<QJsonValue, QJsonArray, QJsonObject>;
    QtJsonSaxConsumer sax;
    QElapsedTimer timer;
    timer.start();
    bool result = json::sax_parse(data, &sax);
    qDebug() << "Nlohmann JSON SAX QJsonObject" << timer.nsecsElapsed();
    setSchema(sax.root);
 */

        QJsonParseError errorPtr{};
        //timer.restart();
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
        //qDebug() << "Qt QJsonDocument" << timer.nsecsElapsed();
        setSchema(root);

/*
    timer.restart();
    const auto &&json = json::parse(data);
    qDebug() << "Nlohmann JSON" << timer.nsecsElapsed();

    timer.restart();
    QJsonDocument &&doc2 = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        qWarning() << "Parsing failed" << errorPtr.error;
        return;
    }
    QJsonObject &&root2 = doc2.object();
    if (root.isEmpty()) {
        qWarning() << "Root is empty.";
        return;
    }
    auto &&map = root2.toVariantMap();
    qDebug() << "QJsonDocument QVariantMap"<< timer.nsecsElapsed();

    QtSaxConsumer sax2;
    timer.restart();
    bool result2 = json::sax_parse(data, &sax);
    qDebug() << "Nlohmann JSON SAX QVariantMap" << timer.nsecsElapsed();

    timer.restart();
    const auto &&json2 = json::parse(data);
    const auto &variant = from_json(json2);
    qDebug() << "Nlohmann JSON QVariant" << timer.nsecsElapsed();

    QFile msgpackFile(finfo.filePath().replace(".min.json", ".msgpack"));
    msgpackFile.open(QIODevice::ReadOnly);
    QByteArray &&msgpackData = msgpackFile.readAll();
    msgpackFile.close();

    timer.restart();
    const auto &&json3 = json::from_msgpack(msgpackData);
    qDebug() << "Nlohmann MsgPack" << timer.nsecsElapsed();

    timer.restart();
    const auto &&json4 = json::from_msgpack(msgpackData);
    const auto &variant2 = from_json(json4);
    qDebug() << "Nlohmann MsgPack QVariant" << timer.nsecsElapsed();

    QtJsonSaxConsumer sax3;
    timer.restart();
    bool result3 = json::sax_parse(msgpackData, &sax3, json::input_format_t::msgpack);
    qDebug() << "Nlohmann MsgPack SAX QJsonObject" << timer.nsecsElapsed();

    QtSaxConsumer sax4;
    timer.restart();
    bool result4 = json::sax_parse(msgpackData, &sax4, json::input_format_t::msgpack);
    qDebug() << "Nlohmann MsgPack SAX QVariantMap" << timer.nsecsElapsed();
 */

        QElapsedTimer timer;
        timer.start();
        loadSchema(filepath);
        qDebug() << "Schema loaded in" << timer.elapsed() << "ms";
    }

    void resolveRedirects(const json &j, Schema::Node *node,
                          Schema::RootNode *root = nullptr) {
        if (node->kind() == Schema::Node::Kind::Root) {
            root = static_cast<Schema::RootNode *>(node);
        }

        if (j.contains("redirect")) {
            Q_ASSERT(root != nullptr);
            // Example: "execute as >@e< at @s ..."
            node->setRedirect(
                root->literalChildren()[j["redirect"][0].get<QString>()]);
        }

        if (j.contains("children")) {
            const auto &children        = j["children"];
            const auto &literalChildren = node->literalChildren();
            for (auto it = literalChildren.cbegin();
                 it != literalChildren.cend(); ++it) {
                resolveRedirects(children[it.key().toStdString()], it.value(),
                                 root);
            }
            const auto &argChildren = node->argumentChildren();
            for (const auto &child: argChildren) {
                resolveRedirects(children[child->name().toStdString()], child,
                                 root);
            }
        } else if (!(node->redirect() || node->isExecutable())) {
            // Example: "execute >run< say ..."
            node->setRedirect(root);
        }
    }

    void Parser::loadSchema(const QString &filepath) {
        QFileInfo finfo(filepath);

        if (!(finfo.exists() && finfo.isFile())) {
            qWarning() << "File not exists:" << finfo.filePath();
            return;
        }

        QFile      f(finfo.filePath());
        const auto openOptions = (finfo.suffix() == "json")
                ? (QIODevice::ReadOnly | QIODevice::Text) : QIODevice::ReadOnly;
        f.open(openOptions);
        QByteArray &&data = f.readAll();
        f.close();

        json j;

        if (finfo.suffix() == "json") {
            j = json::parse(data);
        } else if (finfo.suffix() == "msgpack") {
            j = json::from_msgpack(data);
        }

        m_schemaGraph = j.get<Schema::RootNode>();
        resolveRedirects(j, &m_schemaGraph);
        // TODO: Merge duplicated sub-trees
    }

/*!
 * \brief Returns a shared pointer to the parsing result as a \c RootNode.
 */
    Parser::Result Parser::parsingResult() {
        return Result{ m_tree, m_spans };
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

/*!
 * \brief Gets the last parse error of the parser.
 */
    Parser::Error Parser::lastError() const {
        return m_lastError;
    }

/*!
 * \brief Gets the current position of the parser.
 */
    int Parser::pos() const {
        return m_pos;
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
 * \brief Returns the next literal string (word) without advancing the current pos.
 */
    QString Parser::peekLiteral() {
        return peekUntil(QChar::Space).toString();
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

/*!
 * \brief Converts the specified parser ID into a method name to use with Qt's meta method system.
 */
    QString Parser::parserIdToMethodName(const QString &str) {
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

    QSharedPointer<BoolNode> Parser::brigadier_bool() {
        const int start = pos();

        if (peek(4) == QLatin1String("true")) {
            advance(4);
            return QSharedPointer<BoolNode>::create(spanText(start), true);
        } else if (peek(5) == QLatin1String("false")) {
            advance(5);
            return QSharedPointer<BoolNode>::create(spanText(start), false);
        } else {
            return nullptr;
        }
    }

    QSharedPointer<DoubleNode> Parser::brigadier_double(
        const QVariantMap &props) {
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
        return QSharedPointer<DoubleNode>::create(spanText(raw), value);
    }

    QSharedPointer<FloatNode> Parser::brigadier_float(
        const QVariantMap &props) {
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
        return QSharedPointer<FloatNode>::create(spanText(raw), value);
    }

    QSharedPointer<IntegerNode> Parser::brigadier_integer(
        const QVariantMap &props) {
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
        return QSharedPointer<IntegerNode>::create(spanText(raw), value);
    }

    QSharedPointer<LiteralNode> Parser::brigadier_literal() {
        const QString &&literal = getWithRegex(m_literalStrRegex);
        const int       typeId  = qMetaTypeId<QSharedPointer<LiteralNode> >();
        CacheKey        key{ typeId, literal };

        if (m_cache.contains(key)) {
            return qSharedPointerCast<LiteralNode>(m_cache[key]);
        } else {
            auto &&ret =
                QSharedPointer<LiteralNode>::create(spanText(literal));
            m_cache.emplace(typeId, literal, ret);
            return ret;
        }
    }

    QSharedPointer<StringNode> Parser::brigadier_string(
        const QVariantMap &props) {
        auto defaultRet = QSharedPointer<StringNode>::create(QString());

        if (!props.contains(QLatin1String("type")))
            error(QT_TR_NOOP(
                      "The required paramenter 'type' of the 'brigadier_string' argument parser is missing."));

        QString &&type = props[QLatin1String("type")].toString();
        if (type == QLatin1String("greedy")) {
            return QSharedPointer<StringNode>::create(spanText(getRest()));
        } else if (type == QLatin1String("phrase")) {
            if (m_curChar == '"' || m_curChar == '\'') {
                const int    start = m_pos;
                const auto &&str   = getQuotedString();
                return QSharedPointer<StringNode>::create(spanText(start), str);
            } else {
                error(QT_TR_NOOP("A quoted string is required."));
            }
        } else if (type == QLatin1String("word")) {
            const QString &&literal = getWithRegex(m_literalStrRegex);
            return QSharedPointer<StringNode>::create(spanText(literal));
        }
        return defaultRet;
    }

/*!
 * \brief Parses the current text using the static schema. Returns the \c parsingResult or an invaild \c ParseNode if an error occured.
 */
    QSharedPointer<ParseNode> Parser::parse() {
        m_tree = QSharedPointer<RootNode>::create();

        if (m_schema.isEmpty()) {
            qWarning() << "The parser schema hasn't been initialized yet.";
        } else if (text().trimmed().isEmpty() ||
                   text().trimmed().at(0) == '#') {
            m_tree->setIsValid(true);
            return m_tree;
        }
        setPos(0);
        const int typeId = qMetaTypeId<QSharedPointer<RootNode> >();
        CacheKey  key{ typeId, m_text };

        if (m_cache.contains(key) && !m_testMode) {
            m_tree = qSharedPointerCast<RootNode>(m_cache[key]);
            setPos(m_text.length());
        } else {
            try {
                m_tree->setLeadingTrivia(skipWs());
                if (parseBySchema(&m_schemaGraph)) {
                    m_tree->setLength(pos() - 1);
                    m_tree->setTrailingTrivia(skipWs());
                    m_tree->setIsValid(true);
                    m_cache.emplace(typeId, m_text, m_tree);
                }
            } catch (const Parser::Error &err) {
                qDebug() << "Command::Parser::parse" <<
                    err.toLocalizedMessage();
                m_lastError = err;
                m_tree->setIsValid(false);
            }
        }
        return m_tree;
    }

    bool Parser::canContinue(Schema::Node **schemaNode, int depth) {
        if (depth > 100)
            qWarning() << "The parsing stack depth is too large:" << depth;

        bool canEndParsing = curChar().isNull() or (peek(2) == " ");
        if ((*schemaNode)->isExecutable() && canEndParsing) {
            return false;
        } else if ((*schemaNode)->isEmpty() && (*schemaNode)->redirect()) {
            *schemaNode = (*schemaNode)->redirect();
        }
        if (curChar().isNull()) {
            error(QT_TR_NOOP("Incompleted command"));
        }
        eat(' ');

        return true;
    }

    bool Parser::parseBySchema(const Schema::Node *schemaNode, int depth) {
        NodePtr                ret = nullptr;
        QVector<Parser::Error> errors;
        QVector<int>           argLengths;

        const bool isRoot = schemaNode->kind() == Schema::Node::Kind::Root;

        int             startPos = m_pos;
        bool            success  = false;
        const QString &&literal  = peekLiteral();

        if (schemaNode->literalChildren().contains(literal)) {
            const auto &&command = brigadier_literal();
            if (isRoot)
                command->setIsCommand(true);
            ret = command;
            Schema::Node *litNode =
                schemaNode->literalChildren().value(literal);
            if (canContinue(&litNode, depth)) {
                ret->setTrailingTrivia(QStringLiteral(" "));
                success = parseBySchema(litNode, depth + 1);
                m_tree->prepend(ret);
            } else {
                m_tree->append(ret);
                return true;
            }
        } else if (schemaNode->argumentChildren().isEmpty()) {
            if (isRoot) {
                errors << Error(QT_TR_NOOP("Unknown command '%1'"),
                                m_pos, literal.length(), { literal });
            } else {
                errors << Error(QT_TR_NOOP("Unknown subcommand '%1'"),
                                m_pos, literal.length(), { literal });
            }
        } else {
            for (const auto *argNode: schemaNode->argumentChildren()) {
                QString    &&parserId   = argNode->parserId();
                const auto &&methodName = parserIdToMethodName(parserId);
                int          methodIndex;
                if (!argNode->properties().isEmpty()) {
                    methodIndex = metaObject()->indexOfMethod(
                        QStringLiteral("%1(QVariantMap)").arg(
                            methodName).toLatin1());
                } else {
                    methodIndex = metaObject()->indexOfMethod(
                        QStringLiteral("%1()").arg(methodName).toLatin1());
                }
                if (methodIndex != -1) {
                    auto   &&method     = metaObject()->method(methodIndex);
                    int      returnType = method.returnType();
                    CacheKey key{ returnType, literal };
                    if (m_cache.contains(key)) {
                        ret = m_cache[key];
                        advance(ret->length());
                    } else {
                        const auto &props = argNode->properties();

                        auto typeName = method.typeName();
                        QGenericReturnArgument
                            returnArgument(typeName,
                                           static_cast<void *>(&ret));

                        try {
                            const bool invoked =
                                method.invoke(this, returnArgument,
                                              Q_ARG(QVariantMap, props));
                            if (invoked) {
                                Q_ASSERT(ret != nullptr);
                            } else {
                                qWarning() << QLatin1String(
                                    "Method not invoked: %1 (%2)").arg(parserIdToMethodName(
                                                                           parserId),
                                                                       parserId);
                            }
                        } catch (const Parser::Error &err) {
                            errors << err;
                            int argLength = m_pos - startPos + 1;
                            setPos(startPos);
                            argLengths << argLength;
                            continue;
                        }
                    }

                    if (ret) {
                        if ((literal.length() == ret->length()) &&
                            !m_testMode)
                            m_cache.emplace(returnType, literal, ret);
                        Schema::Node *node =
                            const_cast<Schema::ArgumentNode *>(argNode);
                        if (canContinue(&node, depth)) {
                            ret->setTrailingTrivia(QStringLiteral(" "));
                            try {
                                success = parseBySchema(node, depth + 1);
                                m_tree->prepend(ret);
                            } catch (const Parser::Error &err) {
                                errors << err;
                                int argLength = m_pos - startPos + 1;
                                setPos(startPos);
                                argLengths << argLength;
                                continue;
                            }
                        } else {
                            m_tree->append(ret);
                            return true;
                        }
                        break;
                    }
                }
            }
        }

        if (ret && success) { /* If succeed */
            return true;
        } else if (!errors.isEmpty()) {
            if (errors.size() == 1) {
                throw errors[0];
            } else if (errors.size() == schemaNode->argumentChildren().size()) {
                throw errors[*std::max_element(argLengths.cbegin(),
                                               argLengths.cend())];
            } else {
                for (const auto &err: errors) {
                    if (!m_errors.contains(err)) {
                        m_errors << err;
                    }
                }
            }
            return false;
        } else {
            Q_UNREACHABLE();
        }
    }
}
