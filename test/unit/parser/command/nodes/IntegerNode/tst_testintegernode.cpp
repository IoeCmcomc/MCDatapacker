#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/integernode.h"

using namespace Command;

class TestIntegerNode : public QObject
{
    Q_OBJECT

public:
    TestIntegerNode();
    ~TestIntegerNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
};

TestIntegerNode::TestIntegerNode() {
}

TestIntegerNode::~TestIntegerNode() {
}

void TestIntegerNode::initTestCase() {
}

void TestIntegerNode::cleanupTestCase() {
}

void TestIntegerNode::test_case1() {
    Command::IntegerNode node(22, 10, 1700290000);

    QCOMPARE(node.toString(), "IntegerNode(1700290000)");
}

QTEST_MAIN(TestIntegerNode)

#include "tst_testintegernode.moc"
