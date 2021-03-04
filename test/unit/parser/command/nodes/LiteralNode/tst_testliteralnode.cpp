#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/literalnode.h"

using namespace Command;

class TestLiteralNode : public QObject
{
    Q_OBJECT

public:
    TestLiteralNode();
    ~TestLiteralNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void text();
    void setText();
};

TestLiteralNode::TestLiteralNode() {
}

TestLiteralNode::~TestLiteralNode() {
}

void TestLiteralNode::initTestCase() {
}

void TestLiteralNode::cleanupTestCase() {
}

void TestLiteralNode::test_case1() {
    LiteralNode node(-1, "test_case1");

    QCOMPARE(node.toString(), "LiteralNode(test_case1)");
}

void TestLiteralNode::text() {
    LiteralNode node(-1, "kill");

    QCOMPARE(node.text(), "kill");
}

void TestLiteralNode::setText() {
    LiteralNode node(-1, "ban");

    QCOMPARE(node.text(), "ban");

    node.setText("pardon");
    QCOMPARE(node.text(), "pardon");
}

QTEST_MAIN(TestLiteralNode)

#include "tst_testliteralnode.moc"
