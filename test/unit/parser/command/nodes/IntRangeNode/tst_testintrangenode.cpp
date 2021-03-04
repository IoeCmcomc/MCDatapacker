#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/intrangenode.h"

using namespace Command;

class TestIntRangeNode : public QObject
{
    Q_OBJECT

public:
    TestIntRangeNode();
    ~TestIntRangeNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void minValue();
    void maxValue();
    void exactValue();
};

TestIntRangeNode::TestIntRangeNode() {
}

TestIntRangeNode::~TestIntRangeNode() {
}

void TestIntRangeNode::initTestCase() {
}

void TestIntRangeNode::cleanupTestCase() {
}

void TestIntRangeNode::test_case1() {
    IntRangeNode node(2, 5);

    node.setMinValue(QSharedPointer<IntegerNode>::create(0, 1, 7), false);
    node.setMaxValue(QSharedPointer<IntegerNode>::create(0, 1, 12), true);

    QCOMPARE(node.toString(), "IntRangeNode(7..12)");
}

void TestIntRangeNode::minValue() {
    IntRangeNode node(0, 0);

    node.setMinValue(QSharedPointer<IntegerNode>::create(0, 1, 1), false);
    QCOMPARE(node.hasMinValue(), true);
    QCOMPARE(node.minValue()->value(), 1);
}

void TestIntRangeNode::maxValue() {
    IntRangeNode node(0, 0);

    node.setMaxValue(QSharedPointer<IntegerNode>::create(0, 1, 22), false);
    QCOMPARE(node.hasMaxValue(), true);
    QCOMPARE(node.maxValue()->value(), 22);
}

void TestIntRangeNode::exactValue() {
    IntRangeNode node(0, 0);

    node.setExactValue(QSharedPointer<IntegerNode>::create(0, 2, -9));
    QCOMPARE(node.hasMinValue(), false);
    QCOMPARE(node.hasMaxValue(), false);
    QCOMPARE(node.exactValue()->value(), -9);
}

QTEST_MAIN(TestIntRangeNode)

#include "tst_testintrangenode.moc"
