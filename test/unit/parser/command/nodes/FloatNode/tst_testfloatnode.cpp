#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/floatnode.h"

using namespace Command;

class TestFloatNode : public QObject
{
    Q_OBJECT

public:
    TestFloatNode();
    ~TestFloatNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
};

TestFloatNode::TestFloatNode() {
}

TestFloatNode::~TestFloatNode() {
}

void TestFloatNode::initTestCase() {
}

void TestFloatNode::cleanupTestCase() {
}

void TestFloatNode::test_case1() {
    FloatNode node(0, 6, 0.7749);

    QCOMPARE(node.toString(), "FloatNode(0.7749)");
}

QTEST_MAIN(TestFloatNode)

#include "tst_testfloatnode.moc"
