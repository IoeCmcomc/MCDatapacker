#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/doublenode.h"

using namespace Command;

class TestDoubleNode : public QObject
{
    Q_OBJECT

public:
    TestDoubleNode();
    ~TestDoubleNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
};

TestDoubleNode::TestDoubleNode() {
}

TestDoubleNode::~TestDoubleNode() {
}

void TestDoubleNode::initTestCase() {
}

void TestDoubleNode::cleanupTestCase() {
}

void TestDoubleNode::test_case1() {
    DoubleNode node(this, 0, 4, 22.4);

    QCOMPARE(node.toString(), "DoubleNode(22.4)");
}

QTEST_MAIN(TestDoubleNode)

#include "tst_testdoublenode.moc"
