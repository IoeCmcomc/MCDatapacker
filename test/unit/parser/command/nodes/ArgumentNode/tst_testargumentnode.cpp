#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/argumentnode.h"

using namespace Command;

class TestArgumentNode : public QObject
{
    Q_OBJECT

public:
    TestArgumentNode();
    ~TestArgumentNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void parserId();
};

TestArgumentNode::TestArgumentNode() {
}

TestArgumentNode::~TestArgumentNode() {
}

void TestArgumentNode::initTestCase() {
}

void TestArgumentNode::cleanupTestCase() {
}

void TestArgumentNode::test_case1() {
    ArgumentNode node(0, 1, "mcdatapacker:test");

    QCOMPARE(node.toString(), "ArgumentNode<mcdatapacker:test>");
}

void TestArgumentNode::parserId() {
    ArgumentNode node(2, 4, "mcdatapacker:id");

    QCOMPARE(node.parserId(), "mcdatapacker:id");
}

QTEST_MAIN(TestArgumentNode)

#include "tst_testargumentnode.moc"
