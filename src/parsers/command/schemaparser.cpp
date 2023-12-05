#include "schemaparser.h"

//#include "visitors/sourceprinter.h"
#include "schema/schemaargumentnode.h"
#include "schema/schemaliteralnode.h"
#include "schema/schemaloader.h"

#include "re2c_generated_functions.h"

#include "uberswitch.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QElapsedTimer>

/* These classes are for benchmarking deserialization methods.
 * For now, deserializing from JSON using QJsonDocument
 * is faster than from MessagePack.

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

const static QHash<QString, QString> misspellings = {
    { "allign",      "align"      },
    { "alwasy",      "always"     },
    { "alwyas",      "always"     },
    { "ammount",     "amount"     },
    { "atribute",    "attribute"  },
    { "availabe",    "available"  },
    { "availaible",  "available"  },
    { "availble",    "available"  },
    { "availiable",  "available"  },
    { "availible",   "available"  },
    { "avalable",    "available"  },
    { "avaliable",   "available"  },
    { "avilable",    "available"  },
    { "bedore",      "before"     },
    { "befoer",      "before"     },
    { "claer",       "clear"      },
    { "dammage",     "damage"     },
    { "destory",     "destroy"    },
    { "difficulity", "difficulty" },
    { "dificulty",   "difficulty" },
    { "doulbe",      "double"     },
    { "entitity",    "entity"     },
    { "everthing",   "everything" },
    { "everyting",   "everything" },
    { "excecute",    "execute"    },
    { "experiance",  "experience" },
    { "expierence",  "experience" },
    { "frome",       "from"       },
    { "fucntion",    "function"   },
    { "funtion",     "function"   },
    { "halp",        "help"       },
    { "iwth",        "with"       },
    { "lastr",       "last"       },
    { "lsat",        "last"       },
    { "nmae",        "name"       },
    { "onyl",        "only"       },
    { "recrod",      "record"     },
    { "rocord",      "record"     },
    { "schedual",    "schedule"   },
    { "stlye",       "style"      },
    { "stpo",        "stop"       },
    { "succsess",    "success"    },
    { "sucess",      "success"    },
    { "sytle",       "style"      },
    { "thru",        "through"    },
    { "timne",       "time"       },
    { "tkae",        "take"       },
    { "untill",      "until"      },
    { "visable",     "visible"    },
    { "whith",       "with"       },
    { "wih",         "with"       },
    { "wiht",        "with"       },
    { "withh",       "with"       },
    { "witht",       "with"       },
    { "witn",        "with"       },
    { "wtih",        "with"       },
};

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
                return brigadier_long(props);
            }
            case ParserType::String: {
                return brigadier_string(props);
            }

            default: {
                return nullptr;
            }
        }
    }

    void SchemaParser::setSchema(Schema::RootNode *schema) {
//        Q_ASSERT(schema != nullptr);
        if (m_schemaGraph) {
            delete m_schemaGraph;
        }
        m_schemaGraph = schema;
    }

/*!
 * \brief Opens a JSON file and loads it into the static schema.
 */
    void SchemaParser::loadSchema(const QString &filepath) {
        QElapsedTimer timer;

        timer.start();

        setSchema(Schema::SchemaLoader(filepath).tree());

        qInfo() << "Command schema loaded in" << timer.elapsed() <<
            "ms (path:" << filepath << ")";
    }

    Schema::RootNode * SchemaParser::schema() {
        return m_schemaGraph;
    }

/*!
 * \brief Returns the next literal string (word) without advancing the current pos.
 */
    QString SchemaParser::peekLiteral() const {
        return peekUntil(QChar::Space).toString();
    }

    QStringView SchemaParser::getLiteralString() {
        const int start = pos();

        while (true) {
            switch (curChar().toLatin1()) {
                case 'e':
                case '_':
                case 'a':
                case 't':
                case 'r':
                case 'n':
                case 'c':
                case 'l':
                case 'm':
                case 'i':
                case 'h':
                case 's':
                case 'o':
                case 'd':
                case 'p':
                case 'b':
                case 'u':
                case 'g':
                case 'j':
                case 'k':
                case 'y':
                case '1':
                case 'f':
                case 'v':
                case 'w':
                case '2':
                case 'z':
                case 'C':
                case '3':
                case 'q':
                case 'x':
                case '4':
                case 'T':
                case '5':
                case 'L':
                case '0':
                case 'N':
                case '.':
                case 'H':
                case 'E':
                case '7':
                case '6':
                case '9':
                case '8':
                case 'S':
                case 'I':
                case 'G':
                case 'O':
                case 'A':
                case 'D':
                case 'W':
                case 'P':
                case 'M':
                case 'F':
                case 'B':
                case 'R':
                case 'K':
                case '-':
                case 'Z':
                case 'V':
                case 'Q':
                case '+':
                case 'J':
                case 'U':
                case 'X':
                case 'Y': {
                    advance();
                    break;
                }
                default: {
                    return textView().mid(start, pos() - start);
                }
            }
        }
    }

    QStringView SchemaParser::getDigits() {
        const int start = pos();

        if (curChar() == '-' || curChar() == '+') {
            advance();
        }
        while (curChar().isDigit()) {
            advance();
        }
        return textView().mid(start, pos() - start);
    }

    QPair<QStringView, int> SchemaParser::parseInteger(bool &ok) {
        const QStringView raw   = getDigits();
        const int         value = strToDec<int>(raw, ok);

        return { raw, value };
    }

    QPair<QStringView, float> SchemaParser::parseFloat(bool &ok) {
        ok = false;
        const QStringView raw   = re2c::decimal(peekRest());
        const float       value = raw.toFloat(&ok);

        if (ok) {
            advance(raw.length());
        }

        return { raw, value };
    }

    QSharedPointer<BoolNode> SchemaParser::brigadier_bool() {
        const int start = pos();

        if (peek(4) == "true"_QL1) {
            advance(4);
            return QSharedPointer<BoolNode>::create(spanText(start), true,
                                                    true);
        } else if (peek(5) == "false"_QL1) {
            advance(5);
            return QSharedPointer<BoolNode>::create(spanText(start), false,
                                                    true);
        } else {
            reportError(QT_TR_NOOP(
                            "A boolean value can only be either 'true' or 'false'"));
            return QSharedPointer<BoolNode>::create(
                spanText(getUntil(QChar::Space)), false);
        }
    }

    QSharedPointer<DoubleNode> SchemaParser::brigadier_double(
        const QVariantMap &props) {
        const QStringView raw   = re2c::decimal(peekRest());
        bool              ok    = false;
        double            value = raw.toDouble(&ok);

        if (!ok) {
            reportError(QT_TR_NOOP("%1 is not a vaild double number"),
                        { raw.toString() });
            return QSharedPointer<DoubleNode>::create(spanText(raw), false);
        } else {
            advance(raw.length());
        }
        if (const QVariant &vari = props.value(QStringLiteral(
                                                   "min")); vari.isValid()) {
            checkMin(value, vari.toDouble());
        }
        if (const QVariant &vari = props.value(QStringLiteral(
                                                   "max")); vari.isValid()) {
            checkMax(value, vari.toDouble());
        }
        return QSharedPointer<DoubleNode>::create(spanText(raw), value, true);
    }

    QSharedPointer<FloatNode> SchemaParser::brigadier_float(
        const QVariantMap &props) {
        const QStringView raw   = re2c::decimal(peekRest());
        bool              ok    = false;
        float             value = raw.toFloat(&ok);

        if (!ok) {
            reportError(QT_TR_NOOP("%1 is not a vaild float number"),
                        { raw.toString() });
            return QSharedPointer<FloatNode>::create(spanText(raw), false);
        } else {
            advance(raw.length());
        }
        if (const QVariant &vari = props.value(QStringLiteral(
                                                   "min")); vari.isValid()) {
            checkMin(value, vari.toFloat());
        }
        if (const QVariant &vari = props.value(QStringLiteral(
                                                   "max")); vari.isValid()) {
            checkMax(value, vari.toFloat());
        }
        return QSharedPointer<FloatNode>::create(spanText(raw), value, true);
    }

    QSharedPointer<IntegerNode> SchemaParser::brigadier_integer(
        const QVariantMap &props) {
        bool ok;

        auto [raw, value] = parseInteger(ok);

        if (!ok) {
            reportError(QT_TR_NOOP("%1 is not a vaild integer number"),
                        { raw.toString() });
            return QSharedPointer<IntegerNode>::create(spanText(raw), false);
        }
        if (const QVariant &vari = props.value(QStringLiteral(
                                                   "min")); vari.isValid()) {
            checkMin(value, vari.toInt());
        }
        if (const QVariant &vari = props.value(QStringLiteral(
                                                   "max")); vari.isValid()) {
            checkMax(value, vari.toInt());
        }
        return QSharedPointer<IntegerNode>::create(spanText(raw), value, true);
    }

    QSharedPointer<LongNode> SchemaParser::brigadier_long(
        const QVariantMap &props) {
        bool              ok;
        const QStringView raw   = getDigits();
        const long long   value = strToDec<long long>(raw, ok);

        if (!ok) {
            reportError(QT_TR_NOOP("%1 is not a vaild long number"),
                        { raw.toString() });
            return QSharedPointer<LongNode>::create(spanText(raw), false);
        }
        if (const QVariant &vari = props.value(QStringLiteral(
                                                   "min")); vari.isValid()) {
            checkMin(value, vari.toLongLong());
        }
        if (const QVariant &vari = props.value(QStringLiteral(
                                                   "max")); vari.isValid()) {
            checkMax(value, vari.toLongLong());
        }
        return QSharedPointer<LongNode>::create(spanText(raw), value, true);
    }

    QSharedPointer<LiteralNode> SchemaParser::brigadier_literal() {
        return QSharedPointer<LiteralNode>::create(
            spanText(getUntil(QChar::Space)));
    }

    QSharedPointer<StringNode> SchemaParser::brigadier_string(
        const QVariantMap &props) {
        if (!props.contains(QStringLiteral("type")))
            throwError(QT_TR_NOOP(
                           "The required paramenter 'type' of the 'brigadier:string' argument parser is missing."));

        const QString &&type = props[QStringLiteral("type")].toString();
        uswitch (type) {
            ucase ("greedy"_QL1): {
                return QSharedPointer<StringNode>::create(spanText(getRest()),
                                                          true);
            }
            ucase ("phrase"_QL1): {
                if (curChar() == '"' || curChar() == '\'') {
                    const int    start = pos();
                    const auto &&str   = getQuotedString();
                    return QSharedPointer<StringNode>::create(
                        spanText(start), str, true);
                } else {
                    /*
                     * Using goto here make the code clearer than
                     * using fall through.
                     */
                    goto SINGLE_WORD;
                }
            }
            ucase ("word"_QL1): {
 SINGLE_WORD:
                const auto literal = getLiteralString();
                return QSharedPointer<StringNode>::create(
                    spanText(literal),
                    errorIfNot(!literal.isEmpty(),
                               QT_TR_NOOP("Invalid empty word.")));
            }
        }
        return QSharedPointer<StringNode>::create(QString(), false);
    }

/*!
 * \brief Parses the current text using the static schema.
 * Returns the \c parsingResult or an invalid \c ParseNode if an error occured.
 */
    QSharedPointer<ParseNode> SchemaParser::parse() {
        m_tree = QSharedPointer<RootNode>::create();
        m_errors.clear();
        if (!m_schemaGraph || m_schemaGraph->isEmpty()) {
            qWarning() << "The parser schema hasn't been initialized yet.";
            return m_tree;
        }

        setPos(0);
        try {
            m_tree->setLeadingTrivia(skipWs(false));
            parseBySchema(m_schemaGraph);

            m_tree->setLength(pos() - 1);
            m_tree->setTrailingTrivia(skipWs(false));
//                const auto &treeChildren = m_tree->children();
//                for (auto it = treeChildren.cbegin();
//                     it != treeChildren.cend(); ++it) {
//                    if (!(*it)->isValid()) {
//                        SourcePrinter printer;
//                        printer.startVisiting((*it).get());
//                        qDebug() << printer.source();
//                        break;
//                    }
//                }
        } catch (const SchemaParser::Error &err) {
            qDebug() << "Command::Parser::parse: errors detected";
            qDebug() << textView();
            m_errors << err;
            for (const auto &error: qAsConst(m_errors)) {
                qDebug() << error.toLocalizedMessage();
            }
        }

//        qDebug() << "Size:" << m_cache.size() << '/' << m_cache.capacity()
//                 << "Total access:" << m_cache.stats().total_accesses()
//                 << ". Total hit:" << m_cache.stats().total_hits()
//                 << ". Total miss:" << m_cache.stats().total_misses()
//                 << ". Hit rate:" << m_cache.stats().hit_rate();

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
            throwError(QT_TR_NOOP("Incompleted command"));
        }
        eat(QChar::Space,
            "Unexpected %1, expecting %2 to separate between commands and arguments");

        return true;
    }
    void SchemaParser::parseBySchema(const Schema::Node *schemaNode,
                                     int depth) {
        Q_ASSERT(schemaNode != nullptr);
        NodePtr ret;

        const bool isRoot = schemaNode->kind() == Schema::Node::Kind::Root;

        const int       start                = pos();
        const QString &&literal              = peekLiteral();
        Schema::Node   *litNode              = nullptr;
        bool            reportInvalidCommand = false;

        const auto &literalChildren = schemaNode->literalChildren();

        if (const auto &literalNode = literalChildren.constFind(literal);
            literalNode != literalChildren.end()) {
            litNode = literalNode.value();

            const auto &&command = brigadier_literal();
            if (isRoot)
                command->setIsCommand(true);
            ret = command;
        } else if (!schemaNode->argumentChildren().isEmpty()) {
            const auto &argumentChildren = schemaNode->argumentChildren();
            const auto *lastArgNode      = argumentChildren.constLast();
            for (const auto *argNode: argumentChildren) {
                const auto parserType = argNode->parserType();
                if (parserType == ArgumentNode::ParserType::Unknown) {
                    reportError(QT_TR_NOOP(
                                    "Cannot parse unsupported argument type"),
                                {}, pos(),
                                literal.length());
                    continue;
                }
                const bool  canBacktrack = argNode != lastArgNode;
                const auto &props        = argNode->properties();
                ret.reset();

                try {
                    ret = invokeMethod(parserType, props);
                    Q_ASSERT(ret != nullptr);
                } catch (const SchemaParser::Error &err) {
//                    err.length = pos() - start + 1;
                    m_errors << err;
                }
                if (!ret || (!ret->isValid() && canBacktrack)) {
                    setPos(start);
                    continue;
                }

                Schema::Node *node =
                    const_cast<Schema::ArgumentNode *>(argNode);
                try {
                    if (canContinue(&node, depth)) {
                        if (argNode->isEmpty() && !argNode->redirect() &&
                            canBacktrack) {
                            setPos(start);
                            continue;
                        }

//                        ret->setTrailingTrivia(QStringLiteral(" "));
                        parseBySchema(node, depth + 1);
                        m_tree->prepend(ret);
                    } else {
                        m_tree->append(ret);
                    }
                } catch (SchemaParser::Error &err) {
//                    err.length = pos() - start + 1;
                    m_errors << err;
                    if (!canBacktrack) {
                        m_tree->append(ret);
                        m_tree->append(QSharedPointer<ErrorNode>::create(
                                           spanText(getRest())));
                    } else {
                        setPos(start);
                    }
                }
                ret->setSchemaNode(argNode);
                break;
            }
        } else {
            reportInvalidCommand = true;
            if (literal.length() > 2) {
                const QStringList &&literals = literalChildren.keys();
                const auto        &&filtered = literals.filter(literal,
                                                               Qt::CaseInsensitive);
                if (!filtered.isEmpty()) {
                    litNode = literalChildren[filtered[0]];
                } else if (misspellings.contains(literal)) {
                    litNode = literalChildren[misspellings[literal]];
                } else {
                    for (const QString &lit: literals) {
                        if (literal.contains(lit)) {
                            litNode = literalChildren[lit];
                            break;
                        }
                    }
                }

                if (litNode) {
                    const auto &&command = brigadier_literal();
                    command->setIsValid(false);
                    if (isRoot)
                        command->setIsCommand(true);
                    ret = command;
                }
            }
        }

        if (litNode) {
            try {
                if (canContinue(&litNode, depth)) {
//                ret->setTrailingTrivia(QStringLiteral(" "));
                    parseBySchema(litNode, depth + 1);
                    m_tree->prepend(ret);
                } else {
                    m_tree->append(ret);
                }
            } catch (SchemaParser::Error &err) {
                //err.length = pos() - start + 1;
                m_errors << err;
                ret->setIsValid(false);
                m_tree->append(ret);
            }
        }

        if (!ret) {
            reportInvalidCommand = true;

            ret = QSharedPointer<ErrorNode>::create(spanText(getRest()));
            if (depth > 0) {
                ret->setLeadingTrivia(" ");
            }
            m_tree->append(ret);
        } else if (depth > 0) {
            ret->setLeadingTrivia(QStringLiteral(" "));
        }

        if (litNode && ret) {
            ret->setSchemaNode(litNode);
        }

        if (reportInvalidCommand) {
            if (literal.isEmpty()) {
                if (isRoot) {
                    const QString &cmdGuide = commandGuideStr(schemaNode);
                    reportError(QT_TR_NOOP("Available commands: %1"),
                                { cmdGuide }, start, literal.length());
                } else {
                    if (!schemaNode->isEmpty() && !schemaNode->redirect()) {
                        const QString &cmdGuide = commandGuideStr(schemaNode);
                        reportError(QT_TR_NOOP("Available sub-commands: %1"),
                                    { cmdGuide }, start,
                                    literal.length());
                    } else {
                        reportError(QT_TR_NOOP("Invalid empty sub-command"), {},
                                    start, literal.length());
                    }
                }
            } else {
                if (isRoot) {
                    if (!schemaNode->isEmpty() && !schemaNode->redirect()) {
                        const QString &cmdGuide = commandGuideStr(schemaNode);
                        reportError(QT_TR_NOOP(
                                        "Unknown command '%1'. Available commands: %2"),
                                    { literal, cmdGuide },
                                    start,
                                    literal.length());
                    } else {
                        reportError(QT_TR_NOOP(
                                        "Unknown command '%1'"), { literal },
                                    start, literal.length());
                    }
                } else {
                    if (!schemaNode->isEmpty() && !schemaNode->redirect()) {
                        const QString &cmdGuide = commandGuideStr(schemaNode);
                        reportError(QT_TR_NOOP(
                                        "Unknown sub-command '%1'. Available sub-commands: %2"),
                                    { literal, cmdGuide },
                                    start,
                                    literal.length());
                    } else {
                        reportError(QT_TR_NOOP("Unknown sub-command '%1'"),
                                    { literal }, start, literal.length());
                    }
                }
            }
        }
    }

    const QString Command::SchemaParser::commandGuideStr(
        const Schema::Node *schemaNode) {
        QStringList subcommands;

        const auto &literalChildren = schemaNode->literalChildren();

        for (auto it = literalChildren.cbegin();
             it != literalChildren.cend(); ++it) {
            subcommands << it.key();
        }

        return subcommands.join(", ");
    }
}
