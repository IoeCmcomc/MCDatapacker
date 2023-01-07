#include <QtTest>
#include <QCoreApplication>

#include "../../../../../src/parsers/command/schemaparser.h"

using namespace Command;

class TestSchemaParser : public QObject
{
    Q_OBJECT

public:
    TestSchemaParser();
    ~TestSchemaParser();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void parseBool();
    void parseDouble();
    void parseFloat();
    void parseInteger();
    void parseString();
    void useRegexToParseStringLiteral();
    void useLoopToParseStringLiteral();
    void useLoopToParseStringLiteral2();
    void useLoopToParseStringLiteral3();
};

TestSchemaParser::TestSchemaParser() {
    qDebug() << "TestParser::TestParser";
}

TestSchemaParser::~TestSchemaParser() {
}

void TestSchemaParser::initTestCase() {
    SchemaParser::setSchema(":/minecraft/1.15/summary/commands/data.min.json");
    Command::SchemaParser::setTestMode(true);
}

void TestSchemaParser::cleanupTestCase() {
}

void TestSchemaParser::test_case1() {
    SchemaParser parser("text");

    QCOMPARE(parser.text(), "text");
}

void TestSchemaParser::parseBool() {
    const QString input("true");

    SchemaParser             parser(input);
    QSharedPointer<BoolNode> result(parser.brigadier_bool());

    QVERIFY(result->isValid());
    QCOMPARE(result->kind(), ParseNode::Kind::Argument);
    QCOMPARE(result->parserType(), ArgumentNode::ParserType::Bool);
    QCOMPARE(result->text(), "true");
    QCOMPARE(result->value(), true);

    parser.setText("false");
    result = QSharedPointer<BoolNode>(parser.brigadier_bool());
    QVERIFY(result->isValid());
    QCOMPARE(result->kind(), ParseNode::Kind::Argument);
    QCOMPARE(result->parserType(), ArgumentNode::ParserType::Bool);
    QCOMPARE(result->text(), "false");
    QCOMPARE(result->value(), false);

    parser.setText("simp");
    result = QSharedPointer<BoolNode>(parser.brigadier_bool());
    QVERIFY(result == nullptr);
}

void TestSchemaParser::parseDouble() {
    SchemaParser               parser("3.1415926535897932");
    QSharedPointer<DoubleNode> result(parser.brigadier_double());

    QVERIFY(result->isValid());
    QCOMPARE(result->kind(), ParseNode::Kind::Argument);
    QCOMPARE(result->parserType(), ArgumentNode::ParserType::Double);
    QCOMPARE(result->text(), "3.1415926535897932");
    QCOMPARE(result->value(), 3.1415926535897932);
}

void TestSchemaParser::parseFloat() {
    SchemaParser              parser("99.9");
    QSharedPointer<FloatNode> result(parser.brigadier_float({ { "max",
                                                                100 } }));

    QVERIFY(result->isValid());
    QCOMPARE(result->kind(), ParseNode::Kind::Argument);
    QCOMPARE(result->parserType(), ArgumentNode::ParserType::Float);
    QCOMPARE(result->text(), "99.9");
    QVERIFY(qFuzzyCompare(result->value(), (float)99.9));
}

void TestSchemaParser::parseInteger() {
    SchemaParser                parser("66771508");
    QSharedPointer<IntegerNode> result(
        parser.brigadier_integer({ { "min", 1000000 } }));

    QVERIFY(result->isValid());
    QCOMPARE(result->kind(), ParseNode::Kind::Argument);
    QCOMPARE(result->parserType(), ArgumentNode::ParserType::Integer);
    QCOMPARE(result->text(), "66771508");
    QCOMPARE(result->value(), 66771508);
}

void TestSchemaParser::parseString() {
    SchemaParser               parser("firstWord secondWord");
    QSharedPointer<StringNode> result(
        parser.brigadier_string({ { "type", "word" } }));

    QVERIFY(result->isValid());
    QCOMPARE(result->value(), "firstWord");

    parser.setText("cho xin it da cuoi");
    result = QSharedPointer<StringNode>(
        parser.brigadier_string({ { "type", "greedy" } }));

    QVERIFY(result->isValid());
    QCOMPARE(result->kind(), ParseNode::Kind::Argument);
    QCOMPARE(result->parserType(), ArgumentNode::ParserType::String);
    QCOMPARE(result->text(), "cho xin it da cuoi");
    QCOMPARE(result->value(), "cho xin it da cuoi");

    parser.setText("\"Speed Upgrade for Blocks\"");
    result = QSharedPointer<StringNode>(
        parser.brigadier_string({ { "type", "phrase" } }));
    QVERIFY(result->isValid());
    QCOMPARE(result->kind(), ParseNode::Kind::Argument);
    QCOMPARE(result->parserType(), ArgumentNode::ParserType::String);
    QCOMPARE(result->text(), "\"Speed Upgrade for Blocks\"");
    QCOMPARE(result->value(), "Speed Upgrade for Blocks");
}

static const QRegularExpression m_literalStrRegex{
    QStringLiteral(R"([\w.+-]+)") };

QStringList parseWithRegex(const QString &text) {
    int   pos = 0;
    QChar chr;

    QStringList ret;

    while (pos < text.length()) {
        chr = text.at(pos);
        const auto &match = m_literalStrRegex.match(text,
                                                    pos,
                                                    QRegularExpression::NormalMatch);

        if (match.hasMatch()) {
            const QString &&matchStr = match.captured();
            pos = match.capturedStart() + matchStr.length();
            ret << std::move(matchStr);
        } else {
            return ret;
        }
    }
    return ret;
}

QStringList parseWithoutRegex(const QString &text) {
    int   pos = 0;
    QChar chr;

    QStringList ret;
    int         start = -1;

    while (pos < text.length()) {
        chr = text.at(pos);
        if (chr.isLetterOrNumber() || chr == '_' || chr == '.' || chr == '-' ||
            chr == '+') {
            if (start == -1) {
                start = pos;
            }
        } else if (start != -1) {
            ret << text.mid(start, pos - start);
            start = -1;
        }
        pos += 1;
    }
    if (start != -1) {
        ret << text.mid(start, pos - start);
    }
    return ret;
}

QStringList parseWithoutRegex2(const QString &text) {
    int   pos = 0;
    QChar chr;

    QStringList ret;
    int         start = -1;

    while (pos < text.length()) {
        chr = text.at(pos);
        switch (chr.toLatin1()) {
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.':
            case '+':
            case '-':
            case '_':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z': {
                if (start == -1) {
                    start = pos;
                }
                break;
            }
            default: {
                if (start != -1) {
                    ret << text.mid(start, pos - start);
                    start = -1;
                }
                break;
            }
        }
        pos += 1;
    }
    if (start != -1) {
        ret << text.mid(start, pos - start);
    }
    return ret;
}

QStringList parseWithoutRegex3(const QString &text) {
    int   pos = 0;
    QChar chr;

    QStringList ret;
    int         start = -1;

    while (pos < text.length()) {
        chr = text.at(pos);
        if ((chr >= 'a' && chr <= 'z') || (chr >= '0' && chr <= '9') ||
            (chr >= 'A' && chr <= 'Z') || chr == '_' || chr == '.' ||
            chr == '-' || chr == '+') {
            if (start == -1) {
                start = pos;
            }
        } else if (start != -1) {
            ret << text.mid(start, pos - start);
            start = -1;
        }
        pos += 1;
    }
    if (start != -1) {
        ret << text.mid(start, pos - start);
    }
    return ret;
}

static const auto input = QStringLiteral(
    "IP addresses are written and displayed in human-readable notations, such as 192.0.2.1 in IPv4, and 2001:db8:0:1234:0:567:8:1 in IPv6. The size of the routing prefix of the address is designated in CIDR notation by suffixing the address with the number of significant bits, e.g., 192.0.2.1/24, which is equivalent to the historically used subnet mask 255.255.255.0.");
static const QStringList result = {
    "IP",             "addresses",   "are",
    "written",        "and",         "displayed", "in",
    "human-readable", "notations",   "such",
    "as",             "192.0.2.1",   "in",
    "IPv4",           "and",         "2001",      "db8",
    "0",              "1234",        "0",
    "567",            "8",           "1",         "in",        "IPv6.",
    "The",            "size",        "of",
    "the",            "routing",     "prefix",    "of",        "the",
    "address",        "is",          "designated","in",
    "CIDR",           "notation",    "by",        "suffixing", "the",
    "address",        "with",        "the",       "number",
    "of",             "significant", "bits",      "e.g.",
    "192.0.2.1",      "24",          "which",
    "is",             "equivalent",  "to",        "the",       "historically",
    "used",           "subnet",      "mask",
    "255.255.255.0.",
};

//static const auto input = QStringLiteral(
//    "a_long_string_with_numbers_such_as_12345");
//static const auto result = QStringList(
//    "a_long_string_with_numbers_such_as_12345");

void TestSchemaParser::useRegexToParseStringLiteral() {
    QStringList ret;

    QBENCHMARK {
        ret = parseWithRegex(input);
    }
    QCOMPARE(ret, result);
}

void TestSchemaParser::useLoopToParseStringLiteral() {
    QStringList ret;

    QBENCHMARK {
        ret = parseWithoutRegex(input);
    }
    QCOMPARE(ret, result);
}

void TestSchemaParser::useLoopToParseStringLiteral2() {
    QStringList ret;

    QBENCHMARK {
        ret = parseWithoutRegex2(input);
    }
    QCOMPARE(ret, result);
}

void TestSchemaParser::useLoopToParseStringLiteral3() {
    QStringList ret;

    QBENCHMARK {
        ret = parseWithoutRegex3(input);
    }
    QCOMPARE(ret, result);
}

QTEST_GUILESS_MAIN(TestSchemaParser)

#include "tst_testschemaparser.moc"
