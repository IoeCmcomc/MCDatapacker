#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/boolnode.h"

using namespace Command;

class TestBoolNode : public QObject
{
    Q_OBJECT

public:
    TestBoolNode();
    ~TestBoolNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void value();
    void setValue();
};

TestBoolNode::TestBoolNode() {
}

TestBoolNode::~TestBoolNode() {
}

void TestBoolNode::initTestCase() {
}

void TestBoolNode::cleanupTestCase() {
}

void TestBoolNode::test_case1() {
    BoolNode node(0, true);

    QCOMPARE(node.toString(), "BoolNode(true)");
}

void TestBoolNode::value() {
    BoolNode node(0, false);

    QCOMPARE(node.value(), false);

    BoolNode node2(41, true);
    QCOMPARE(node2.value(), true);
}

void TestBoolNode::setValue() {
    BoolNode node(0, true);

    QCOMPARE(node.value(), true);

    node.setValue(false);
    QCOMPARE(node.value(), false);
}

QTEST_MAIN(TestBoolNode)

#include "tst_testboolnode.moc"
