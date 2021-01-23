#include <QtTest/QTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/parsenode.h"

using namespace Command;

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
    void pos();
    void length();
    void isVaild();
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

void TestParseNode::pos() {
    ParseNode node(this, 5);

    QCOMPARE(node.pos(), 5);

    node.setPos(24);
    QCOMPARE(node.pos(), 24);
}

void TestParseNode::length() {
    ParseNode node(this, 0, 3);

    QCOMPARE(node.length(), 3);

    ParseNode node2(this, 1, 15);
    QCOMPARE(node2.length(), 15);
}

void TestParseNode::isVaild() {
    ParseNode node(this);

    QCOMPARE(node.isVaild(), false);

    node.setPos(0);
    QCOMPARE(node.isVaild(), true);
}

QTEST_GUILESS_MAIN(TestParseNode)

#include "tst_testparsenode.moc"
