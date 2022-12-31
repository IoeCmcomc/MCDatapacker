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

QTEST_GUILESS_MAIN(TestSchemaParser)

#include "tst_testschemaparser.moc"
