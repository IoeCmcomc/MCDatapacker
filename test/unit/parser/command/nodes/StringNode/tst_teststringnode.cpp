#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/stringnode.h"

using namespace Command;

class TestStringNode : public QObject
{
    Q_OBJECT

public:
    TestStringNode();
    ~TestStringNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
};

TestStringNode::TestStringNode() {
}

TestStringNode::~TestStringNode() {
}

void TestStringNode::initTestCase() {
}

void TestStringNode::cleanupTestCase() {
}

void TestStringNode::test_case1() {
    StringNode node(
        "Although a \"long\" argument type is present in brigadier, it is not used by Minecraft.",
        true);

    QVERIFY(node.isValid());
    QCOMPARE(node.kind(), ParseNode::Kind::Argument);
    QCOMPARE(node.parserType(), ArgumentNode::ParserType::String);
    QCOMPARE(node.length(), 85);
    QCOMPARE(
        node.text(),
        "Although a \"long\" argument type is present in brigadier, it is not used by Minecraft.");
    QCOMPARE(
        node.value(),
        "Although a \"long\" argument type is present in brigadier, it is not used by Minecraft.");
}

QTEST_MAIN(TestStringNode)

#include "tst_teststringnode.moc"
