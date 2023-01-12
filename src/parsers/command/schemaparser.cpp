#include "schemaparser.h"

#include "schema/schemaargumentnode.h"
#include "schema/schemaliteralnode.h"

#include "uberswitch/include/uberswitch/uberswitch.hpp"

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
    SchemaParser::SchemaParser() {
    }

    NodePtr SchemaParser::invokeMethod(ArgumentNode::ParserType parserType,
                                       const QVariantMap &props) {
        using ParserType = ArgumentNode::ParserType;
        switch (parserType) {
            case ParserType::Bool: {
                return brigadier_bool();
            }
            case ParserType::Double: {
                return brigadier_double(props);
            }
            case ParserType::Float: {
                return brigadier_float(props);
            }
            case ParserType::Integer: {
                return brigadier_integer(props);
            }
            case ParserType::Literal: {
                return brigadier_literal();
            }
            case ParserType::Long: {
                error("The 'brigadier:long' parser has'nt been implemented yet.");
            }
            case ParserType::String: {
                return brigadier_string(props);
            }

            default: {
                return nullptr;

                break;
            }
        }
    }

    QJsonObject SchemaParser::getSchema() {
        return m_schema;
    }

    void SchemaParser::setSchema(const QJsonObject &schema) {
        m_schema = schema;
    }

/*!
 * \brief Opens a JSON file and loads it into the static schema.
 */
    void SchemaParser::setSchema(const QString &filepath) {
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

    void SchemaParser::loadSchema(const QString &filepath) {
        QFileInfo finfo(filepath);

        if (!(finfo.exists() && finfo.isFile())) {
            qWarning() << "File not exists:" << finfo.filePath();
            return;
        }

        QFile      f(finfo.filePath());
        const bool isJson      = finfo.suffix() == "json"_QL1;
        const auto openOptions = (isJson)
                ? (QIODevice::ReadOnly | QIODevice::Text) : QIODevice::ReadOnly;
        f.open(openOptions);
        QByteArray &&data = f.readAll();
        f.close();

        json j;

        if (isJson) {
            j = json::parse(data);
        } else if (finfo.suffix() == "msgpack"_QL1) {
            j = json::from_msgpack(data);
        }

        m_schemaGraph = j.get<Schema::RootNode>();
        resolveRedirects(j, &m_schemaGraph);
        // TODO: Merge duplicated sub-trees
    }

/*!
 * \brief Returns a shared pointer to the parsing result as a \c RootNode.
 */
    SchemaParser::Result SchemaParser::parsingResult() {
        return Result{ m_tree, m_spans };
    }


/*!
 * \brief Returns the next literal string (word) without advancing the current pos.
 */
    QString SchemaParser::peekLiteral() {
        return peekUntil(QChar::Space).toString();
    }

    QSharedPointer<BoolNode> SchemaParser::brigadier_bool() {
        const int start = pos();

        if (peek(4) == "true"_QL1) {
            advance(4);
            return QSharedPointer<BoolNode>::create(spanText(start), true);
        } else if (peek(5) == "false"_QL1) {
            advance(5);
            return QSharedPointer<BoolNode>::create(spanText(start), false);
        } else {
            return nullptr;
        }
    }

    QSharedPointer<DoubleNode> SchemaParser::brigadier_double(
        const QVariantMap &props) {
        const QString &&raw   = getWithRegex(m_decimalNumRegex);
        bool            ok    = false;
        double          value = raw.toDouble(&ok);

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

    QSharedPointer<FloatNode> SchemaParser::brigadier_float(
        const QVariantMap &props) {
        const QString &&raw   = getWithRegex(m_decimalNumRegex);
        bool            ok    = false;
        float           value = raw.toFloat(&ok);

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

    QSharedPointer<IntegerNode> SchemaParser::brigadier_integer(
        const QVariantMap &props) {
        const QString &&raw   = getWithRegex(QStringLiteral(R"([+-]?\d+)"));
        bool            ok    = false;
        int             value = raw.toFloat(&ok);

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

    QSharedPointer<LiteralNode> SchemaParser::brigadier_literal() {
//        static constexpr int typeId  = getTypeEnumId<LiteralNode>();
        const QString &&literal = getUntil(QChar::Space).toString();
//        CacheKey             key{ typeId, literal };

//        if (m_cache.contains(key)) {
//            return qSharedPointerCast<LiteralNode>(m_cache[key]);
//        } else {
        const auto &&ret =
            QSharedPointer<LiteralNode>::create(spanText(literal));

//        m_cache.emplace(typeId, literal, ret);
        return ret;
//        }
    }

    QSharedPointer<StringNode> SchemaParser::brigadier_string(
        const QVariantMap &props) {
        const auto &&defaultRet = QSharedPointer<StringNode>::create(QString());

        if (!props.contains(QStringLiteral("type")))
            error(QT_TR_NOOP(
                      "The required paramenter 'type' of the 'brigadier_string' argument parser is missing."));

        const QString &&type = props[QStringLiteral("type")].toString();
        uswitch (type) {
            ucase ("greedy"_QL1): {
                return QSharedPointer<StringNode>::create(spanText(getRest()));
            }
            ucase ("phrase"_QL1): {
                if (curChar() == '"' || curChar() == '\'') {
                    const int    start = pos();
                    const auto &&str   = getQuotedString();
                    return QSharedPointer<StringNode>::create(spanText(start),
                                                              str);
                } else {
                    error(QT_TR_NOOP("A quoted string is required."));
                }
            }
            ucase ("word"_QL1): {
                const QString &&literal = getWithRegex(m_literalStrRegex);
                return QSharedPointer<StringNode>::create(spanText(literal));
            }
        }
        return defaultRet;
    }

/*!
 * \brief Parses the current text using the static schema. Returns the \c parsingResult or an invalid \c ParseNode if an error occured.
 */
    QSharedPointer<ParseNode> SchemaParser::parse() {
        m_tree = QSharedPointer<RootNode>::create();
        m_errors.clear();
        const QString &txt = text();

        if (m_schema.isEmpty()) {
            qWarning() << "The parser schema hasn't been initialized yet.";
            m_tree->setIsValid(false);
            return m_tree;
        }

        setPos(0);
//        constexpr int typeId = getTypeEnumId<RootNode>();
//        CacheKey      key{ typeId, text() + '\1' };

//        if (m_cache.contains(key) && !m_testMode) {
//            m_tree = qSharedPointerCast<RootNode>(m_cache[key]);
//            setPos(txt.length());
//        } else {
        try {
            m_tree->setLeadingTrivia(skipWs());
            if (parseBySchema(&m_schemaGraph)) {
                m_tree->setLength(pos() - 1);
                m_tree->setTrailingTrivia(skipWs());
                m_tree->setIsValid(true);
//                    m_cache.emplace(typeId, txt, m_tree);
            }
        } catch (const SchemaParser::Error &err) {
            //qDebug() << "Command::Parser::parse: errors detected";
            m_errors << err;
//            for (const auto &error: m_errors) {
//                qDebug() << error.toLocalizedMessage();
//            }
            m_tree->setIsValid(false);
        }
//        }
        return m_tree;
    }

    const ParseNodeCache &SchemaParser::cache() const {
        return m_cache;
    }

    void SchemaParser::setTestMode(bool value) {
        SchemaParser::m_testMode = value;
    }

    bool SchemaParser::canContinue(Schema::Node **schemaNode, int depth) {
        if (depth > 256)
            qWarning() << "The parsing stack depth is too large:" << depth;

        const bool canEndParsing = curChar().isNull()
                                   || (peek(2) == " "_QL1);
        if ((*schemaNode)->isExecutable() && canEndParsing) {
            return false;
        } else if ((*schemaNode)->isEmpty() && (*schemaNode)->redirect()) {
            *schemaNode = (*schemaNode)->redirect();
        }
        if (curChar().isNull()) {
            error(QT_TR_NOOP("Incompleted command"));
        }
        eat(QChar::Space);

        return true;
    }

    bool SchemaParser::parseBySchema(const Schema::Node *schemaNode,
                                     int depth) {
        NodePtr                      ret;
        QVector<SchemaParser::Error> errors;

        const bool isRoot = schemaNode->kind() == Schema::Node::Kind::Root;

        int             startPos = pos();
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
                                pos(), literal.length(), { literal });
            } else {
                errors << Error(QT_TR_NOOP("Unknown subcommand '%1'"),
                                pos(), literal.length(), { literal });
            }
        } else {
            for (const auto *argNode: schemaNode->argumentChildren()) {
                const auto parserType = argNode->parserType();
                if (parserType == ArgumentNode::ParserType::Unknown) {
                    errors <<
                        Error(QT_TR_NOOP("Cannot parse unsupported argument"),
                              pos(), literal.length(), {});
                    continue;
                }
//                const int   parserId = static_cast<int>(parserType);
                const auto &props = argNode->properties();


//                CacheKey key{ parserId, literal, props };
//                if (m_cache.contains(key)) {
//                    ret = m_cache[key];
//                    advance(ret->length());
//                } else {
                try {
                    ret = invokeMethod(parserType, props);
                    Q_ASSERT(ret != nullptr);
                } catch (SchemaParser::Error &err) {
                    err.length = pos() - startPos + 1;
                    errors << err;
                    setPos(startPos);
                    continue;
                }
//                }

                if (ret) {
//                    if ((literal.length() == ret->length()) &&
//                        !m_testMode)
//                        m_cache.emplace(parserId, literal, ret);
                    Schema::Node *node =
                        const_cast<Schema::ArgumentNode *>(argNode);
                    if (canContinue(&node, depth)) {
                        ret->setTrailingTrivia(QStringLiteral(" "));
                        try {
                            success = parseBySchema(node, depth + 1);
                            m_tree->prepend(ret);
                        } catch (SchemaParser::Error &err) {
                            err.length = pos() - startPos + 1;
                            errors << err;
                            setPos(startPos);
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

        if (ret && success) { /* If succeed */
            return true;
        } else if (!errors.isEmpty()) {
            if (errors.size() == 1) {
                throw errors[0];
            } else if (errors.size() == schemaNode->argumentChildren().size()) {
                const auto errorToThrow = *std::max_element(
                    errors.cbegin(), errors.cend(),
                    [](const auto& a, const auto& b) {
                    return a.length < b.length;
                });
                throw errorToThrow;
            } else {
                for (const auto &err: errors) {
                    if (!m_errors.contains(err)) {
                        m_errors << err;
                    }
                }
            }
            return true;
        } else {
            Q_UNREACHABLE();
        }
    }
}
