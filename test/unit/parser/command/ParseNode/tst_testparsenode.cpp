#include <QtTest/QTest>
#include <QCoreApplication>

#include "../../../../../src/parsers/command/parsenode.h"

using namespace CommandParser;

class TestParseNode : public QObject
{
    Q_OBJECT

public:
    TestParseNode();
    ~TestParseNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
};

TestParseNode::TestParseNode() {
}

TestParseNode::~TestParseNode() {
}

void TestParseNode::initTestCase() {
}

void TestParseNode::cleanupTestCase() {
}

void TestParseNode::test_case1() {
    ParseNode node(this);

    QCOMPARE(node.toString(), "ParseNode()");
    /* QVERIFY(node.toString() == "ParseNode()"); */
}

QTEST_GUILESS_MAIN(TestParseNode)

#include "tst_testparsenode.moc"
