#include <QtTest>
#include <QCoreApplication>

#include "../../../../../src/parsers/command/parser.h"
#include "lru-cache/include/lru/lru.hpp"

using namespace Command;

class TestParser : public QObject
{
    Q_OBJECT

public:
    TestParser();
    ~TestParser();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void parseBool();
    void parseDouble();
    void parseFloat();
    void parseInteger();
    void parseString();
    void parse();
    void parserIdToMethodName();
};

TestParser::TestParser() {
    qDebug() << "TestParser::TestParser";
}

TestParser::~TestParser() {
}

void TestParser::initTestCase() {
    Parser::setSchema(
        ":/minecraft/1.15/summary/commands/data.min.json");
    Command::Parser::setTestMode(true);
}

void TestParser::cleanupTestCase() {
}

void TestParser::test_case1() {
    Parser parser(this, "text");

    QCOMPARE(parser.text(), "text");
}

void TestParser::parseBool() {
    Parser                   parser(this, "true");
    QSharedPointer<BoolNode> result(parser.brigadier_bool());

    QVERIFY(result->isVaild());
    QCOMPARE(result->value(), true);

    parser.setText("false");
    result = QSharedPointer<BoolNode>(parser.brigadier_bool());
    QVERIFY(result->isVaild());
    QCOMPARE(result->value(), false);

    parser.setText("false");
    result = QSharedPointer<BoolNode>(parser.brigadier_bool());
    QVERIFY(result->isVaild());
    QCOMPARE(result->value(), false);


    parser.setText("simp");
    result = QSharedPointer<BoolNode>(parser.brigadier_bool());
    QVERIFY(!result->isVaild());
}

void TestParser::parseDouble() {
    Parser                     parser(this, "3.1415926535897932");
    QSharedPointer<DoubleNode> result(parser.brigadier_double());

    QVERIFY(result->isVaild());
    QCOMPARE(result->value(), 3.1415926535897932);
}

void TestParser::parseFloat() {
    Parser                    parser(this, "99.9");
    QSharedPointer<FloatNode> result(parser.brigadier_float({ { "max",
                                                                100 } }));

    QVERIFY(result->isVaild());
    QVERIFY(qFuzzyCompare(result->value(), (float)99.9));
}

void TestParser::parseInteger() {
    Parser                      parser(this, "66771508");
    QSharedPointer<IntegerNode> result(
        parser.brigadier_integer({ { "min", 1000000 } }));

    QVERIFY(result->isVaild());
    QCOMPARE(result->value(), 66771508);
}

void TestParser::parse() {
    QElapsedTimer timer;

    timer.start();

    Parser parser(this, "gamemode creative");

    auto result = parser.parse();
    QCOMPARE(result->toString(),
             "RootNode[2](LiteralNode(gamemode), LiteralNode(creative))");

    parser.setText("schedule clear test");
    result = parser.parse();
    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(schedule), LiteralNode(clear), StringNode(\"test\"))");

    parser.setText("gamerule keepInventory true");
    result = parser.parse();
    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(gamerule), LiteralNode(keepInventory), BoolNode(true))");

    parser.setText("gamerule doWeatherCycle false");
    result = parser.parse();
    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(gamerule), LiteralNode(doWeatherCycle), BoolNode(false))");

    parser.setText("gamerule doDaylightCycle");
    result = parser.parse();
    QCOMPARE(result->toString(),
             "RootNode[2](LiteralNode(gamerule), LiteralNode(doDaylightCycle))");

    qDebug() << "Elapsed time in ms:" << timer.elapsed();
}

void TestParser::parserIdToMethodName() {
    QCOMPARE(Parser::parserIdToMethodName("brigadier:float"),
             "brigadier_float");
    QCOMPARE(Parser::parserIdToMethodName("minecraft:block_pos"),
             "minecraft_blockPos");
    QCOMPARE(Parser::parserIdToMethodName("minecraft:nbt_compound_tag"),
             "minecraft_nbtCompoundTag");
}

void TestParser::parseString() {
    Parser                     parser(this, "firstWord secondWord");
    QSharedPointer<StringNode> result(
        parser.brigadier_string({ { "type", "word" } }));

    QVERIFY(result->isVaild());
    QCOMPARE(result->value(), "firstWord");

    parser.setText("cho xin it da cuoi");
    result = QSharedPointer<StringNode>(
        parser.brigadier_string({ { "type", "greedy" } }));
    QVERIFY(result->isVaild());
    QCOMPARE(result->value(), "cho xin it da cuoi");

    parser.setText("\"Speed Upgrade for Blocks\"");
    result = QSharedPointer<StringNode>(
        parser.brigadier_string({ { "type", "phrase" } }));
    QVERIFY(result->isVaild());
    QCOMPARE(result->value(), "Speed Upgrade for Blocks");
}

/*QTEST_GUILESS_MAIN(TestParser) */

int main(int argc, char *argv[]) {
    TESTLIB_SELFCOVERAGE_START("TestParser")
    QT_PREPEND_NAMESPACE(QTest::Internal::callInitMain) < TestParser > ();
    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    TestParser tc;
    QTEST_SET_MAIN_SOURCE_PATH
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_testparser.moc"
